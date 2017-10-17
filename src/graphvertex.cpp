#include "graphvertex.h"

// Initialize data
CGraphVertex::CGraphVertex() : 
  m_vPoint{ 0, 0, 0 }, 
  m_iIndex{-1}
{}

// Set some initial value using 
CGraphVertex::CGraphVertex(const double* pValue) : 
  m_vPoint{ pValue[0], pValue[1], pValue[2] }, 
  m_iIndex{ -1 }
{}

// Set some initial values and the id
CGraphVertex::CGraphVertex(int iIndex, const double* pValue) : 
  m_vPoint{ pValue[0], pValue[1], pValue[2] }, 
  m_iIndex{ iIndex }
{}

// Adding all values manually
CGraphVertex::CGraphVertex(int iIndex, const double& dX, double dY, double dZ) : 
  m_vPoint{ dX, dY, dZ }, 
  m_iIndex{ iIndex }
{}

CGraphVertex::CGraphVertex(const CGraphVertex &v)
{
  m_vPoint[0] = v.m_vPoint[0];
  m_vPoint[1] = v.m_vPoint[1];
  m_vPoint[2] = v.m_vPoint[2];
  m_iIndex = v.m_iIndex;
}

void CGraphVertex::setIndex(const int& index)
{
  this->m_iIndex = index;
}