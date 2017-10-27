#include "navigation.h"
#include <iterator>
#include <algorithm>
#include <vtkCardinalSpline.h>
#include <vtkSplineFilter.h>
/*
CNavigation::CNavigation() 
{

}

CNavigation::~CNavigation() 
{

}
*/
// Load the file with skeleton points
// the format is x y z, one per line
// @param theData where add points
bool LoadSkelPoints(const std::string & strFilename, Graph& theData)
{
  ifstream ifFile(strFilename);
  if (ifFile.is_open())
  {
    int index = 0;
    while (!ifFile.eof())
    {
      CGraphVertex vertex;
      ifFile >> vertex[1] >> vertex[0] >> vertex[2];
      vertex.setIndex(index++);
      theData.addGraphVertex(vertex);
    }
    ifFile.close();
    theData.computeDistances();	//calculate distance between vertexes
    return true;
  }
  return false;
}

bool CNavigation::openTXTFile(const std::string & strFilename)
{
  if (!LoadSkelPoints(strFilename, m_graph))
    return false;
  return true;
}


void CNavigation::computePath(const int& index)
{
  cout << "Status: Computing the shortest distance from node " << index << " ..." << std::endl;
  m_vPathParent = m_graph.shortestPath(index);
}

void CNavigation::computeMST(const int& index)
{
  cout << "Status: Computing MST path ..." << std::endl;
  m_vPathMST = m_graph.primtMST(index);
}

bool CNavigation::pathInGraph(int source, int destination, std::vector<int>& path)
{
  if (m_vPathParent[destination] == -1) //path does not exists
    return false;

  if (m_vPathParent[destination] == source) //base case
    return true;    //path found

  bool r =  pathInGraph(source, m_vPathParent[destination], path);
  path.push_back(destination);
  return r;
}

void CNavigation::pathInMST(int source, int destination, std::vector<int>& path) 
{
  if (m_vPathMST[destination] != -1)  //no parent
  {
    if (m_vPathMST[destination] != source)  //base case
    {
      path.push_back(destination);
      pathInMST(source, m_vPathMST[destination], path);
    }
  }
}

vtkSmartPointer<vtkPolyData> CNavigation::getSmoothPath(const int & i, const int &j)
{
  //compute the nodes involves in the path, starting from i and finished in j (included)
  std::vector<int> path;
  if (pathInGraph(i, j, path))
    path.insert(path.begin(), i);	//insert the first one at the top, to complete the path
  else
  {
    cout << "There is no a path between " << i << " and " << j << endl;
    return nullptr;
  }

  // spline object
  //vtkSmartPointer<vtkParametricSpline> spline = vtkSmartPointer<vtkParametricSpline>::New();
  //spline->SetPoints(m_graph.getPointsPath(path));	//get the points in the dijsktra's path

  //// function source
  //vtkSmartPointer<vtkParametricFunctionSource> functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  //functionSource->SetParametricFunction(spline);
  //functionSource->Update();

  /////////////////////////////
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  
  vtkSmartPointer<vtkPoints> points = m_graph.getPointsPath(path);
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
  vtkIdType sizePoints = points->GetNumberOfPoints();
  lines->InsertNextCell(sizePoints);
  for (unsigned int i = 0; i < sizePoints; ++i)
    lines->InsertCellPoint(i);
  
  polyData->SetLines(lines);
  polyData->SetPoints(points);
  
  vtkSmartPointer<vtkCardinalSpline> newSpline = vtkSmartPointer<vtkCardinalSpline>::New();
  newSpline->SetLeftConstraint(2);
  newSpline->SetLeftValue(0.0);
  newSpline->SetRightConstraint(2);
  newSpline->SetRightValue(0.0);

  vtkSmartPointer<vtkSplineFilter> splineFilter = vtkSmartPointer<vtkSplineFilter>::New();
  splineFilter->SetInputData(polyData);
  
  splineFilter->SetNumberOfSubdivisions(polyData->GetNumberOfPoints() * 10);
  splineFilter->SetSpline(newSpline);
  splineFilter->Update();

  vtkSmartPointer<vtkPolyData> result = splineFilter->GetOutput();
  cout << "points: " << result->GetNumberOfPoints() << endl;
  cout << "lins: " << result->GetNumberOfLines() << endl;
  return result;

  //the smooth output to follow
  //vtkSmartPointer<vtkPolyData> smoothPath = vtkSmartPointer<vtkPolyData>::New();
  //smoothPath.TakeReference(functionSource->GetOutput());	//this is the smooth path to extract the points to follow a path with camera
  //smoothPath.TakeReference(functionSource->GetOutput()->New());
  //smoothPath = functionSource->GetOutput();
  //smoothPath->CopyStructure(functionSource->GetOutput());
  
  
  //return functionSource->GetOutput();
  
  
  //return smoothPath;
}

//mitk::DataNode::Pointer CNavigation::getMSTDrawingPath()
//{
//  // Create the MITK surface object
//  mitk::Surface::Pointer lines_surface = mitk::Surface::New();
//  lines_surface->SetVtkPolyData(m_graph.getPolyMSTComplete(m_vPathMST));
//
//  // Create a new node in DataNode with properties
//  mitk::DataNode::Pointer result = mitk::DataNode::New();
//  result->SetColor(0, 0.56, 1);
//  std::string nameOfOuputImage = "complete-path-MST";
//  result->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));
//
//  lines_surface->Update();
//  result->SetData(lines_surface);
//  result->SetFloatProperty("material.wireframeLineWidth", 3); //3 as width of the line
//  return result;
//}
//mitk::DataNode::Pointer CNavigation::getMSTDrawingPath(const int & i, const int & j) 
//{
//  //compute the nodes involves in the path, starting from i and finished in j (included)
//  std::vector<int> path;
//  pathInMST(i, j, path);
//  // Create the MITK surface object
//  mitk::Surface::Pointer lines_surface = mitk::Surface::New();
//  lines_surface->SetVtkPolyData(m_graph.getMSTDataPath(path));
//
//  // Create a new node in DataNode with properties
//  mitk::DataNode::Pointer result = mitk::DataNode::New();
//  result->SetColor(1, 0.56, 0);
//  std::string nameOfOuputImage = "path-MST";
//  result->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));
//
//  lines_surface->Update();
//  result->SetData(lines_surface);
//  result->SetFloatProperty("material.wireframeLineWidth", 3); //3 as width of the line
//  return result;
//}
//

//
//mitk::DataNode::Pointer CNavigation::getSmoothDrawingPath(vtkSmartPointer<vtkPolyData> path, std::string name)
//{
//  
//  // Create the MITK surface object
//  mitk::Surface::Pointer lines_surface = mitk::Surface::New();
//  lines_surface->SetVtkPolyData(path);
//
//  // Create a new node in DataNode with properties
//  mitk::DataNode::Pointer result = mitk::DataNode::New();
//  result->SetColor(0.3, 0.3, 0.8);
//  result->SetProperty("name", mitk::StringProperty::New(name));
//
//  lines_surface->Update();
//  result->SetData(lines_surface);
//  result->SetFloatProperty("material.wireframeLineWidth", 3); //3 as width of the line
//  return result;
//
//}
//

vtkSmartPointer<vtkPolyData> CNavigation::getDrawingPath(const int & i, const int & j, std::string name) 
{
  assert(m_vPathParent.size()); //the computePath should be invoked first
  vtkSmartPointer<vtkPolyData> completePath = vtkSmartPointer<vtkPolyData>::New();

  //compute the nodes involves in the path, starting from i and finished in j (included)
  std::vector<int> path;
  if (pathInGraph(i, j, path))
      path.insert(path.begin(), i); //insert the first one at the top, to complete the path
  else 
  {
    cout << "There is no a path between " << i << " and " << j << endl;
    return nullptr;
  }

  //m_graph.get
  return m_graph.getPolyDataPath(path);

  return completePath;
}

//mitk::DataNode::Pointer CNavigation::getDrawingPath(const int & i, const int & j, std::string name)
//{
//  assert(m_vPathParent.size()); //the computePath should be invoked first
//
//  //compute the nodes involves in the path, starting from i and finished in j (included)
//  std::vector<int> path;
//  if (pathInGraph(i, j, path))
//      path.insert(path.begin(), i); //insert the first one at the top, to complete the path
//  else 
//  {
//    cout << "There is no a path between " << i << " and " << j << endl;
//    return nullptr;
//  }
//  
//  //std::for_each(path.begin(), path.end(), [](int i) {cout << i << " "; });  //debug path
//
//  // Create the MITK surface object
//  mitk::Surface::Pointer lines_surface = mitk::Surface::New();
//  lines_surface->SetVtkPolyData(m_graph.getPolyDataPath(path));
//
//  // Create a new node in DataNode with properties
//  mitk::DataNode::Pointer result = mitk::DataNode::New();
//  //result->SetColor(0.1, 0.6, 0.9);
//  result->SetColor(1.0, 0.6, 0.9);
//  result->SetProperty("name", mitk::StringProperty::New(name));
//  
//  lines_surface->Update();
//  result->SetData(lines_surface);
//  result->SetFloatProperty("material.wireframeLineWidth", 3); //3 as width of the line
//  return result;
//}