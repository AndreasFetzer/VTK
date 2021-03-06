/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractBlock.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkExtractBlock - extracts blocks from a multiblock dataset.
// .SECTION Description
// vtkExtractBlock is a filter that extracts blocks from a multiblock dataset.
// Each node in the multi-block tree is identified by an \c index. The index can
// be obtained by performing a preorder traversal of the tree (including empty
// nodes). eg. A(B (D, E), C(F, G)).
// Inorder traversal yields: A, B, D, E, C, F, G
// Index of A is 0, while index of C is 4.

#ifndef __vtkExtractBlock_h
#define __vtkExtractBlock_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"

class vtkCompositeDataIterator;
class vtkMultiPieceDataSet;

class VTKFILTERSEXTRACTION_EXPORT vtkExtractBlock : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkExtractBlock* New();
  vtkTypeMacro(vtkExtractBlock, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Select the block indices to extract.
  // Each node in the multi-block tree is identified by an \c index. The index can
  // be obtained by performing a preorder traversal of the tree (including empty
  // nodes). eg. A(B (D, E), C(F, G)).
  // Inorder traversal yields: A, B, D, E, C, F, G
  // Index of A is 0, while index of C is 4.
  void AddIndex(unsigned int index);
  void RemoveIndex(unsigned int index);
  void RemoveAllIndices();

  // Description:
  // When set, the output mutliblock dataset will be pruned to remove empty
  // nodes. On by default.
  vtkSetMacro(PruneOutput, int);
  vtkGetMacro(PruneOutput, int);
  vtkBooleanMacro(PruneOutput, int);

  // Description:
  // This is used only when PruneOutput is ON. By default, when pruning the
  // output i.e. remove empty blocks, if node has only 1 non-null child block,
  // then that node is removed. To preserve these parent nodes, set this flag to
  // true. Off by default.
  vtkSetMacro(MaintainStructure, int);
  vtkGetMacro(MaintainStructure, int);
  vtkBooleanMacro(MaintainStructure, int);

//BTX
protected:
  vtkExtractBlock();
  ~vtkExtractBlock();

  // Description:
  // Internal key, used to avoid pruning of a branch.
  static vtkInformationIntegerKey* DONT_PRUNE();

  /// Implementation of the algorithm.
  virtual int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *);


  /// Extract subtree
  void CopySubTree(vtkCompositeDataIterator* loc,
    vtkMultiBlockDataSet* output, vtkMultiBlockDataSet* input);
  bool Prune(vtkMultiBlockDataSet* mblock);
  bool Prune(vtkMultiPieceDataSet* mblock);
  bool Prune(vtkDataObject* mblock);

  int PruneOutput;
  int MaintainStructure;
private:
  vtkExtractBlock(const vtkExtractBlock&); // Not implemented.
  void operator=(const vtkExtractBlock&); // Not implemented.

  class vtkSet;
  vtkSet *Indices;
  vtkSet *ActiveIndices;
//ETX
};

#endif


