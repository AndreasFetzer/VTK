/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMathTextActor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMathTextActor.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkMathTextUtilities.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTextProperty.h"
#include "vtkTexture.h"
#include "vtkViewport.h"

// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkMathTextActor);

// ----------------------------------------------------------------------------
bool vtkMathTextActor::IsSupported()
{
  return vtkMathTextUtilities::GetInstance() != NULL;
}

// ----------------------------------------------------------------------------
vtkMathTextActor::vtkMathTextActor()
{
  // Copied from vtkTextActor:
  // To remain compatible with code using vtkActor2D, we must set
  // position coord to Viewport, not Normalized Viewport
  // so...compute equivalent coords for initial position
  this->PositionCoordinate->SetCoordinateSystemToViewport();

  // Construct texture
  this->RectanglePoints->Allocate(4);
  this->Rectangle->SetPoints(this->RectanglePoints.GetPointer());
  vtkNew<vtkCellArray> polys;
  polys->InsertNextCell(4);
  polys->InsertCellPoint(0);
  polys->InsertCellPoint(1);
  polys->InsertCellPoint(2);
  polys->InsertCellPoint(3);
  this->Rectangle->SetPolys(polys.GetPointer());

  vtkNew<vtkFloatArray> tc;
  tc->SetNumberOfComponents(2);
  tc->SetNumberOfTuples(4);
  tc->InsertComponent(0,0, 0.0);  tc->InsertComponent(0,1, 0.0);
  tc->InsertComponent(1,0, 0.0);  tc->InsertComponent(1,1, 1.0);
  tc->InsertComponent(2,0, 1.0);  tc->InsertComponent(2,1, 1.0);
  tc->InsertComponent(3,0, 1.0);  tc->InsertComponent(3,1, 0.0);
  this->Rectangle->GetPointData()->SetTCoords(tc.GetPointer());

  vtkNew<vtkPolyDataMapper2D> mapper;
  this->SetMapper(mapper.GetPointer());
  mapper->SetInputData(this->Rectangle.GetPointer());

  this->TextProperty = vtkSmartPointer<vtkTextProperty>::New();
}

// ----------------------------------------------------------------------------
vtkMathTextActor::~vtkMathTextActor()
{
}

// ----------------------------------------------------------------------------
const char* vtkMathTextActor::GetInput()
{
  return this->Input.c_str();
}

// ----------------------------------------------------------------------------
double *vtkMathTextActor::GetBounds()
{
  this->ComputeRectangle();
  return this->RectanglePoints->GetBounds();
}

// ----------------------------------------------------------------------------
void vtkMathTextActor::SetTextProperty(vtkTextProperty *p)
{
  this->TextProperty = p;
}

// ----------------------------------------------------------------------------
vtkTextProperty *vtkMathTextActor::GetTextProperty()
{
  return this->TextProperty.GetPointer();
}

// ----------------------------------------------------------------------------
void vtkMathTextActor::ShallowCopy(vtkProp *prop)
{
  vtkMathTextActor *a = vtkMathTextActor::SafeDownCast(prop);
  if ( a != NULL )
    {
    this->SetTextProperty(a->GetTextProperty());
    this->SetInput(a->GetInput());
    }
  this->vtkActor2D::ShallowCopy(prop);
}

// ----------------------------------------------------------------------------
void vtkMathTextActor::ReleaseGraphicsResources(vtkWindow *win)
{
  this->vtkActor2D::ReleaseGraphicsResources(win);
  this->Texture->ReleaseGraphicsResources(win);
}

// ----------------------------------------------------------------------------
int vtkMathTextActor::RenderOverlay(vtkViewport *viewport)
{
  if (!this->Visibility)
    {
    return 0;
    }

  // render the texture
  if (this->Texture.GetPointer() && !this->Input.empty())
    {
    vtkRenderer* ren = vtkRenderer::SafeDownCast(viewport);
    if (ren)
      {
      this->Texture->Render(ren);
      }
    }

  // Everything is built in RenderOpaqueGeometry, just have to render
  return this->vtkActor2D::RenderOverlay(viewport);
}

// ----------------------------------------------------------------------------
int vtkMathTextActor::RenderOpaqueGeometry(vtkViewport *viewport)
{
  if (!this->Visibility)
    {
    return 0;
    }

  // Make sure we have a string to render
  if (this->Input.empty())
    {
    return 0;
    }

  //check if we need to render the string

  if(this->TextProperty->GetMTime() > this->ImageData->GetMTime() ||
     this->GetMTime() > this->ImageData->GetMTime())
    {
    unsigned int dpi = 120;
    vtkRenderer *ren = vtkRenderer::SafeDownCast(viewport);
    if (ren)
      {
      if (ren->GetRenderWindow())
        {
        dpi = static_cast<unsigned int>(ren->GetRenderWindow()->GetDPI());
        }
      }

    if (!vtkMathTextUtilities::GetInstance()->RenderString(
          this->Input.c_str(), this->ImageData.GetPointer(),
          this->TextProperty.GetPointer(), dpi))
      {
      vtkErrorMacro(<<"Failed rendering text to buffer");
      return 0;
      }

    this->ComputeRectangle();

    this->ImageData->Modified();
    this->Texture->SetInputData(this->ImageData.GetPointer());
    this->Texture->Modified();
    }

  // Everything is built, just have to render
  return this->vtkActor2D::RenderOpaqueGeometry(viewport);
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkMathTextActor::HasTranslucentPolygonalGeometry()
{
  return 0;
}

// ----------------------------------------------------------------------------
void vtkMathTextActor::ComputeRectangle()
{
  int dims[3];
  this->RectanglePoints->Reset();
  this->ImageData->GetDimensions(dims);

  // compute TCoords.
  vtkFloatArray* tc = vtkFloatArray::SafeDownCast
      ( this->Rectangle->GetPointData()->GetTCoords() );
  tc->InsertComponent(0, 0, 0.0);
  tc->InsertComponent(0, 1, 0.0);

  tc->InsertComponent(1, 0, 0.0);
  tc->InsertComponent(1, 1, 1.0);

  tc->InsertComponent(2, 0, 1.0);
  tc->InsertComponent(2, 1, 1.0);

  tc->InsertComponent(3, 0, 1.0);
  tc->InsertComponent(3, 1, 0.0);

  // Rotate the rectangle
  double radians = vtkMath::RadiansFromDegrees(
        this->TextProperty->GetOrientation());
  double c = cos( radians );
  double s = sin( radians );
  double xo, yo;
  double x, y;
  xo = yo = 0.0;
  switch ( this->TextProperty->GetJustification() )
    {
    default:
    case VTK_TEXT_LEFT:
      break;
    case VTK_TEXT_CENTERED:
      xo = -dims[0] * 0.5;
      break;
    case VTK_TEXT_RIGHT:
      xo = -dims[0];
    }
  switch ( this->TextProperty->GetVerticalJustification())
    {
    default:
    case VTK_TEXT_BOTTOM:
      break;
    case VTK_TEXT_CENTERED:
      yo = -dims[1] * 0.5;
      break;
    case VTK_TEXT_TOP:
      yo = -dims[1];
      break;
    }
  // handle line offset
  yo += this->TextProperty->GetLineOffset();

  x = xo;
  y = yo;
  this->RectanglePoints->InsertNextPoint( c*x-s*y, s*x+c*y, 0.0 );
  x = xo;
  y = yo + dims[1];
  this->RectanglePoints->InsertNextPoint( c*x-s*y, s*x+c*y, 0.0 );
  x = xo + dims[0];
  y = yo + dims[1];
  this->RectanglePoints->InsertNextPoint( c*x-s*y, s*x+c*y, 0.0 );
  x = xo + dims[0];
  y = yo;
  this->RectanglePoints->InsertNextPoint( c*x-s*y, s*x+c*y, 0.0 );
}

// ----------------------------------------------------------------------------
void vtkMathTextActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if (this->TextProperty.GetPointer())
    {
    os << indent << "Text Property:\n";
    this->TextProperty->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Text Property: (none)\n";
    }

  os << indent << "Image Data:\n";
  this->ImageData->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Input:\n"
     << indent.GetNextIndent() << this->Input.c_str() << "\n";

  os << indent << "Rectangle:\n";
  this->Rectangle->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Rectangle Points:\n";
  this->RectanglePoints->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Texture:\n";
  this->Texture->PrintSelf(os,indent.GetNextIndent());
}
