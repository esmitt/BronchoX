#pragma once
#include "vtkPoints.h"
#include "utility.h"
#include <vtkMath.h>
#include <set>
#include <vector>
#include <algorithm>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkGlyph3D.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>

// @author Esmitt Ramirez
// @date 10/2017

/// Struct to handle a graph's vertex. It stores an ID, a 3D point, and a set of linked neighbours
struct GraphVertex
{
private:
	int m_iIndex;	//index of the GraphVertex
	std::set <std::pair<int, double>> m_vNeighbours;	//store a pair of values (id, distance) into a set
	double m_Point[3];	//point 3D

public:
	GraphVertex()
	{
		m_Point[0] = m_Point[1] = m_Point[2] = 0;
		m_iIndex = -1;
	}

	GraphVertex(double* value)
	{
		m_Point[0] = value[0]; m_Point[1] = value[1]; m_Point[2] = value[2];
		m_iIndex = -1;
	}

	GraphVertex(int index, double* value)
	{
		m_Point[0] = value[0]; m_Point[1] = value[1]; m_Point[2] = value[2];
		m_iIndex = index;
	}

	GraphVertex(int index, double x, double y, double z)
	{
		m_Point[0] = x; m_Point[1] = y; m_Point[2] = z;
		m_iIndex = index;
	}

	GraphVertex(const GraphVertex &v)
	{
		m_Point[0] = v.m_Point[0];
		m_Point[1] = v.m_Point[1];
		m_Point[2] = v.m_Point[2];
		m_iIndex = v.m_iIndex;
	}

	~GraphVertex() {}

	//functions
	double & operator[](int index) { return m_Point[index]; }

	//add an edge
	void link(int index, double distance = 0)
	{
		m_vNeighbours.insert(std::make_pair(index, distance));
	}

	inline int getIndex() { return m_iIndex; }

	double* get_ptr() { return m_Point; }

	std::set<std::pair<int, double>>& getNeighbours()
	{
		return m_vNeighbours;
	}

	void printNeighbours()
	{
		auto iIt = m_vNeighbours.begin();
		while (iIt != m_vNeighbours.end())
		{
			std::cout << "(" << m_iIndex << ", " << iIt->first << ") = " << iIt->second << endl;
			iIt++;
		}
	}

  //testing
  bool isNear(const double p[3])
  {
    if (vtkMath::Distance2BetweenPoints(p, m_Point) < 0.1001f)
      return true;
    return false;
  }
};

/// Class to handle a directed/undirected graph using an adjacency list.
/// The vertexes are GraphVertex datatype, also stores vtkPoints to render
class Graph
{
private:
	std::vector<GraphVertex> m_vGraphVertexes;
	vtkSmartPointer<vtkPoints> m_vtkPoints;
  typedef std::pair<int, int> tEdge;	//to define an edge as the link between two nodes represented by their indexes
                                      //a new graph using the MST (reduced from the original)
  std::vector<GraphVertex> m_vGraphMST;

public:
	void addGraphVertex(GraphVertex v);
	void constructVTKPoints();
	void addEdgeIndex(int index1, int index2);

  // Compute the euclidean distance between all pair of nodes nodes/vertexes
	void computeDistances(double epsilon = CUtility::getInstance()->RADIUS_DISTANCE);	// Compute the euclidean distance between all pair of nodes nodes/vertexes

  // Get the points  associated with the path in the vector
  vtkSmartPointer<vtkPoints> getPointsPath(std::vector<int> path);
	
  //// Return a MITK node to be added into the drawing pipeline
	//mitk::DataNode::Pointer getDrawableLines();

	//// Return a MITK node to be added into the drawing pipeline
	//mitk::DataNode::Pointer getDrawablePoints();

  //// Return a VTK structure to draw into the current drawing pipeline
  vtkSmartPointer<vtkPolyData> getDrawablePoints();

  // Find the shortest path from node to all nodex
  std::vector<int> shortestPath(const int & indexI = 0);

  std::vector<int> primtMST(int src = 0);
  
  //vtkSmartPointer<vtkPolyData> getMSTDataPath(std::vector<int> path);	//MST data
  //vtkSmartPointer<vtkPolyData> getPolyMSTComplete(std::vector<int> path); // MST data


  //return the ID in the graph closer to the point (x,y,z) inside the graph (if exists)
  int getID(double x, double y, double z)
  {
    for (size_t i = 0; i < m_vGraphVertexes.size(); i++)
    {
      double p[3] = { x, y, z };
      if (m_vGraphVertexes[i].isNear(p))
        return m_vGraphVertexes[i].getIndex();
    }
    return -1;
  }

  vtkSmartPointer<vtkPoints> getPoints()
  {
    return m_vtkPoints;
  }

	//print for testing
	void print()
	{
		std::for_each(m_vGraphVertexes.begin(), m_vGraphVertexes.end(), [](GraphVertex v)
		{
			std::cout << v.getIndex() << std::endl;
		});
	}
	Graph();
	~Graph();
};

//std::vector<Vertex>::iterator findVertexIndex(double* val, bool& res)
//{
//	std::vector<Vertex>::iterator it;
//	Vertex v(val);
//	it = std::find(m_vVertexes.begin(), m_vVertexes.end(), v);
//	if (it != m_vVertexes.end()) {
//		res = true;
//		return it;
//	}
//	else {
//		res = false;
//		return m_vVertexes.end();
//	}
//}

//void addEdge(int n1, int n2)
//{
//	bool foundNet1 = false, foundNet2 = false;
//	auto vit1 = findVertexIndex(n1, foundNet1);
//	int node1Index = -1, node2Index = -1;
//	if (!foundNet1)
//	{
//		Vertex v1(n1);
//		m_vVertexes.push_back(v1);
//		node1Index = m_vVertexes.size() - 1;
//	}
//	else
//	{
//		node1Index = vit1 - m_vVertexes.begin();
//	}
//	Vertices::iterator vit2 = findVertexIndex(n2, foundNet2);
//	if (!foundNet2)
//	{
//		Vertex v2(n2);
//		m_vVertexes.push_back(v2);
//		node2Index = m_vVertexes.size() - 1;
//	}
//	else
//	{
//		node2Index = vit2 - vertices.begin();
//	}

//	assert((node1Index > -1) && (node1Index <  vertices.size()));
//	assert((node2Index > -1) && (node2Index <  vertices.size()));

//	addEdgeIndices(node1Index, node2Index);
//}