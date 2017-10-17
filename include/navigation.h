#define _NAVIGATION_H_


#include <string>
#include <vtkSmartPointer.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkKochanekSpline.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include "graph.h"

class CNavigation 
{
private:
  Graph m_graph;
  std::vector<int> m_vPathParent;
  std::vector<int> m_vPathMST;
  vtkSmartPointer<vtkPolyData> m_vPathSmooth;
protected:
  bool pathInGraph(int source, int destination, std::vector<int>& path);
  void pathInMST(int source, int destination, std::vector<int>& path);

public:
  CNavigation() = default;
  virtual ~CNavigation() = default;

  // Open a TXT file
  bool openTXTFile(const std::string & strFilename);

  // Compute the shortest distance from vertex-index
  void computePath(const int& index = 0);

  void computeMST(const int& index = 0);

  //// Return the datanode which contains the path between node i and node j
  //mitk::DataNode::Pointer getDrawingPath(const int & i, const int & j, std::string name);
  //
  vtkSmartPointer<vtkPolyData> getSmoothPath(const int & i, const int &j);
  //// Return the datanode which contains the smooth path between node i and node j
  //mitk::DataNode::Pointer getSmoothDrawingPath(const vtkSmartPointer<vtkPolyData> smoothPath, std::string name);

  //mitk::DataNode::Pointer getMSTDrawingPath(const int & i, const int & j);
  //mitk::DataNode::Pointer getMSTDrawingPath();

  //// Return the datanode which contains the points in the skeleton
  vtkSmartPointer<vtkPolyData> getDrawablePoints() { return m_graph.getDrawablePoints(); }
  
  //// Return the datanode which contains the connected lines between nodes
  //mitk::DataNode::Pointer getDrawableLines() { return m_graph.getDrawableLines(); }

  vtkSmartPointer<vtkPolyData> getDrawingPath(const int & i, const int & j, std::string name);
  
  //testing
  Graph* getGraph() { return &m_graph; }
};