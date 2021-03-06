/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMathTextActor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMathTextActor - An actor that displays math text
//
// .SECTION Description
// vtkMathTextActor can be used to place mathtext annotation into a window.
// Set the text property/attributes through the vtkTextProperty associated to
// this actor.
//
// .SECTION See Also
// vtkTextActor vtkMathTextUtilities vtkContext2D

#ifndef __vtkMathTextActor_h
#define __vtkMathTextActor_h

#include "vtkRenderingMathTextModule.h" // For export macro
#include "vtkActor2D.h"
#include "vtkNew.h" // For smart pointer stuff
#include "vtkSmartPointer.h" // For smart pointer stuff
#include "vtkStdString.h" // For vtkStdString

class vtkTextProperty;
class vtkImageData;
class vtkPoints;
class vtkPolyData;
class vtkTexture;

class VTKRENDERINGMATHTEXT_EXPORT vtkMathTextActor : public vtkActor2D
{
public:
  vtkTypeMacro(vtkMathTextActor,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkMathTextActor *New();

  // Description:
  // Returns true if MathText is available on this build of VTK. If false,
  // this actor will not function.
  bool IsSupported();

  // Description:
  // Shallow copy of this actor.
  void ShallowCopy(vtkProp *prop);

  // Description:
  // Set the text string to be displayed.
  vtkSetMacro(Input, const char *);
  const char *GetInput();

  // Description:
  // Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
  // in world coordinates. NULL means that the bounds are not defined.
  double *GetBounds();

  // Description:
  // Set/Get the text property.
  virtual void SetTextProperty(vtkTextProperty *p);
  virtual vtkTextProperty *GetTextProperty();

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Draw the text actor to the screen.
  int RenderOpaqueGeometry(vtkViewport* viewport);
  int RenderTranslucentPolygonalGeometry(vtkViewport* ) { return 0; }
  int RenderOverlay(vtkViewport* viewport);

  // Description:
  // Does this prop have some translucent polygonal geometry?
  int HasTranslucentPolygonalGeometry();

protected:
  vtkMathTextActor();
  ~vtkMathTextActor();

  void ComputeRectangle();

  vtkSmartPointer<vtkTextProperty> TextProperty;
  vtkNew<vtkImageData> ImageData;
  vtkNew<vtkPolyData> Rectangle;
  vtkNew<vtkPoints> RectanglePoints;
  vtkNew<vtkTexture> Texture;
  vtkStdString Input;

private:
  vtkMathTextActor(const vtkMathTextActor&);  // Not implemented.
  void operator=(const vtkMathTextActor&);  // Not implemented.
};

#endif
