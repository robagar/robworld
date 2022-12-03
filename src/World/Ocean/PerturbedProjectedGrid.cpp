#include "../../stdafx.h"
#include "PerturbedProjectedGrid.h"

using namespace Ogre;

PerturbedProjectedGrid::PerturbedProjectedGrid(PerturbedHorizontal* pSurface, 
                                               Ogre::Camera* pRealRenderCam, 
                                               int widthVerts, int heightVerts, 
                                               Ogre::ManualObject* pManualObject, 
                                               const Ogre::String& materialName,
                                               Real minProjectorHeight,
                                               Real maxLookAtPointDistance)
    : m_pRealRenderCam(pRealRenderCam),
      Grid(
        widthVerts, heightVerts,
        pManualObject,
        materialName
      ),
      m_pSurface(pSurface),
      m_minProjectorHeight(minProjectorHeight),
      m_maxLookAtPointDistance(maxLookAtPointDistance),
      m_rangeConversionMatrix(Matrix4::IDENTITY)
{
  SceneManager* pSceneManager = pRealRenderCam->getSceneManager();
  m_pRenderCam = new Ogre::Camera("Projected Grid Render Cam", pSceneManager);
  m_pProjectorCam = new Ogre::Camera("Projected Grid Cam", pSceneManager);

  m_vertexPositions.resize(widthVerts * heightVerts);
}

PerturbedProjectedGrid::~PerturbedProjectedGrid()
{
  delete m_pRenderCam;
  delete m_pProjectorCam;
}

void PerturbedProjectedGrid::update(Ogre::Real time, Ogre::Real deltaTime)
{
  updateProjector();

  if (!determineVisibility())
  {
    m_pManualObject->setVisible(false);
    return;
  }

  positionVerticesOnBasePlane();
  perturbVertices(time, deltaTime);

  m_pManualObject->beginUpdate(0);

  makeVertices();
  triangulate();

  m_pManualObject->end();
  m_pManualObject->setVisible(true);

  AxisAlignedBox b = m_pManualObject->getBoundingBox();
}

void PerturbedProjectedGrid::updateProjector()
{
  m_pRenderCam->synchroniseBaseSettingsWith(m_pRealRenderCam);
  m_pRenderCam->setPosition(m_pRealRenderCam->getRealPosition());
  m_pRenderCam->setOrientation(m_pRealRenderCam->getRealOrientation());
  //m_pRenderCam->setFarClipDistance(100000.0f);

  m_pProjectorCam->synchroniseBaseSettingsWith(m_pRenderCam);
  m_pProjectorCam->setFarClipDistance(100000.0f);
  m_pProjectorCam->setFixedYawAxis(false);

  m_unProjectionMatrix = m_pProjectorCam->getProjectionMatrix() * m_pProjectorCam->getViewMatrix(); 
  m_projectionMatrix = m_unProjectionMatrix.inverse();

  // position above surface
  Vector3 cameraPosition = m_pProjectorCam->getRealPosition();
  Real minZ = m_pSurface->BaseHeight + m_minProjectorHeight;
  if (cameraPosition.z < minZ)
    m_pProjectorCam->setPosition(cameraPosition.x, cameraPosition.y, minZ);

  // determine look at point

  // fixed distance point to look at if looking towards the horizon
  Vector3 positionOnSurface(cameraPosition.x, cameraPosition.y, m_pSurface->BaseHeight);
  Vector3 cameraDirection = m_pRenderCam->getDirection();
  
  if (cameraDirection.z > 0)
    cameraDirection.z = - cameraDirection.z;
  
  Vector3 projectedDirection(cameraDirection.x, cameraDirection.y, 0);
  projectedDirection.normalise();
  Vector3 horizontalLookAtPoint = positionOnSurface + m_maxLookAtPointDistance * projectedDirection;

  m_projectorLookAtPoint = horizontalLookAtPoint;

  // the point on the surface the main camera is looking at
  Ray ray(m_pRenderCam->getRealPosition(), cameraDirection);
  std::pair<bool, Real> i = ray.intersects(m_pSurface->Base);
  if (i.first)
  {
    Vector3 cameraLookAtPoint = ray.getPoint(i.second);
    m_projectorLookAtPoint = lerp(horizontalLookAtPoint, cameraLookAtPoint, cameraDirection.dotProduct(Vector3::NEGATIVE_UNIT_Z));
  }

  m_pProjectorCam->lookAt(m_projectorLookAtPoint);
  m_pProjectorCam->getRealPosition();

  m_projectionMatrix = (m_pProjectorCam->getProjectionMatrix() * m_pProjectorCam->getViewMatrix()).inverse();
}

bool PerturbedProjectedGrid::determineVisibility()
{
  std::vector<Vector3> buffer;

  Vector3 corners[8];

  /*
    Frustum near

    2       3
     +-----+
     |     |
     |     |
     +-----+
    0       1

  */

  corners[0] = Vector3(-1, -1, -1);
  corners[1] = Vector3(+1, -1, -1);
  corners[2] = Vector3(-1, +1, -1);
  corners[3] = Vector3(+1, +1, -1);


  /*
    Frustum far

    6       7
     +-----+
     |     |
     |     |
     +-----+
    4       5

  */


  corners[4] = Vector3(-1, -1, +1);
  corners[5] = Vector3(+1, -1, +1);
  corners[6] = Vector3(-1, +1, +1);
  corners[7] = Vector3(+1, +1, +1);

  // transform corners to world space
  Matrix4 m = (m_pRenderCam->getProjectionMatrix() * m_pRenderCam->getViewMatrix()).inverse();
  for(int i=0; i<8; ++i)
    corners[i] = m * corners[i]; 

  // add any corners within the displacement volume to buffer
  for(int i=0; i<8; ++i)
  {
    Vector3& c = corners[i];
    if (m_pSurface->contains(c))
      buffer.push_back(c);
  }

  // frustum edges
  Ray edges[12];

  // back
  edges[0] = Ray(corners[0], corners[1] - corners[0]);  
  edges[1] = Ray(corners[0], corners[2] - corners[0]);  
  edges[2] = Ray(corners[3], corners[2] - corners[3]);  
  edges[3] = Ray(corners[3], corners[1] - corners[3]);  

  // sides
  edges[4] = Ray(corners[0], corners[4] - corners[0]);  
  edges[5] = Ray(corners[1], corners[5] - corners[1]);  
  edges[6] = Ray(corners[2], corners[6] - corners[2]);  
  edges[7] = Ray(corners[3], corners[7] - corners[3]);  

  // front
  edges[8] = Ray(corners[4], corners[5] - corners[4]);  
  edges[9] = Ray(corners[4], corners[6] - corners[4]);  
  edges[10] = Ray(corners[7], corners[6] - corners[7]);  
  edges[11] = Ray(corners[7], corners[5] - corners[7]);  

  // add all intersections to buffer
  for(int i=0; i<12; ++i)
  {
    Ray& e = edges[i];
    std::pair<bool, Real> p;

    p = e.intersects(m_pSurface->Lower);
    if (p.first && p.second >= 0 && p.second <= 1)
      buffer.push_back(e.getPoint(p.second));

    p = e.intersects(m_pSurface->Upper);
    if (p.first && p.second >= 0 && p.second <= 1)
      buffer.push_back(e.getPoint(p.second));
  }

  // no points in buffer => surface not in view
  if (buffer.empty())
    return false;

  Matrix4 unProjectionMatrix = m_pProjectorCam->getProjectionMatrix() * m_pProjectorCam->getViewMatrix();
  m_rangeMin = Vector2(9999,9999);
  m_rangeMax = Vector2(-9999,-9999);
  foreach(Vector3& p, buffer)
  {
    // project points onto surface
    p.z = m_pSurface->BaseHeight;

    // transform into projector space
    p = unProjectionMatrix * p;

    // find minimum & maximum projector space x and y values
    m_rangeMin = Vector2(min(m_rangeMin.x, p.x), min(m_rangeMin.y, p.y));
    m_rangeMax = Vector2(max(m_rangeMax.x, p.x), max(m_rangeMax.y, p.y));
  }

  // update range conversion matrix
  m_rangeConversionMatrix = Matrix4::IDENTITY;
  m_rangeConversionMatrix[0][0] = m_rangeMax.x - m_rangeMin.x;
  m_rangeConversionMatrix[1][1] = m_rangeMax.y - m_rangeMin.y;
  m_rangeConversionMatrix[0][3] = m_rangeMin.x;
  m_rangeConversionMatrix[1][3] = m_rangeMin.y;

  return true; 
}

void PerturbedProjectedGrid::positionVerticesOnBasePlane()
{
  positionVerticesByInterpolationHomogeneous();
}

void PerturbedProjectedGrid::perturbVertices(Ogre::Real time, Ogre::Real deltaTime)
{
  foreach(Vector3& v, m_vertexPositions)
    m_pSurface->perturb(v, time, deltaTime);
}

void PerturbedProjectedGrid::positionVerticesByInterpolationHomogeneous()
{
  Vector4 bottomLeft  = projectHomogeneous(m_rangeMin);  
  Vector4 bottomRight = projectHomogeneous(Vector2(m_rangeMax.x, m_rangeMin.y));  
  Vector4 topLeft     = projectHomogeneous(Vector2(m_rangeMin.x, m_rangeMax.y));  
  Vector4 topRight    = projectHomogeneous(m_rangeMax);  

  for(int j=0; j<m_heightVerts; ++j)
  {
    float fUp = ((float)j / (m_heightVerts - 1));
    Vector4 left = bottomLeft + fUp * (topLeft - bottomLeft);
    Vector4 right = bottomRight + fUp * (topRight - bottomRight);
    Vector4 deltaLeftToRight = right - left;
    for(int i=0; i<m_widthVerts; ++i)
    {
      float fAcross = ((float) i / (m_widthVerts - 1));
      Vector4 ph = left +  fAcross * deltaLeftToRight;

      Vector3& p = vertexPosition(i, j);
      p.x = ph.x / ph.w; 
      p.y = ph.y / ph.w; 
      p.z = ph.z / ph.w; 
    }
  }
}

void PerturbedProjectedGrid::positionVerticesByUVInterpolationHomogeneous()
{

  //Vector4 bottomLeft  = projectUVHomogeneous(Vector2(0, 0));  
  //Vector4 bottomRight = projectUVHomogeneous(Vector2(1, 0));  
  //Vector4 topLeft     = projectUVHomogeneous(Vector2(0,  1));  
  //Vector4 topRight    = projectUVHomogeneous(Vector2(1,  1));  

  //for(int j=0; j<m_heightVerts; ++j)
  //{
  //  float fUp = ((float)j / (m_heightVerts - 1));
  //  Vector4 left = bottomLeft + fUp * (topLeft - bottomLeft);
  //  Vector4 right = bottomRight + fUp * (topRight - bottomRight);
  //  Vector4 deltaLeftToRight = right - left;
  //  for(int i=0; i<m_widthVerts; ++i)
  //  {
  //    float fAcross = ((float) i / (m_widthVerts - 1));
  //    Vector4 p = left +  fAcross * deltaLeftToRight;

  //    m_pManualObject->position(p.x / p.w, p.y / p.w, p.z /p.w);
  //  }
  //}
}

void PerturbedProjectedGrid::makeVertices()
{
  for(int j=0; j<m_heightVerts; ++j)
  {
    for(int i=0; i<m_widthVerts; ++i)
    {
      m_pManualObject->position(vertexPosition(i,j));
      m_pManualObject->normal(calculateNormal(i,j));
    }
  }
}

Vector3 PerturbedProjectedGrid::calculateNormal(uint32 i, uint32 j)
{
  Vector3& p = vertexPosition(i,j);

  Vector3 north = (j < m_heightVerts-1) ? vertexPosition(i,   j+1) - p : Vector3::ZERO;
  Vector3 east  = (i < m_widthVerts-1)  ? vertexPosition(i+1, j) - p   : Vector3::ZERO;
  Vector3 south = (j > 0)               ? vertexPosition(i,   j-1) - p : Vector3::ZERO;
  Vector3 west  = (i > 0)               ? vertexPosition(i-1, j) - p   : Vector3::ZERO;

  Vector3 n = east.crossProduct(north) + west.crossProduct(south);
  n.normalise();

  return n;
}

String PerturbedProjectedGrid::getDebugText()
{
  std::ostringstream o;

  o << "Projector position: " << m_pProjectorCam->getRealPosition() << endl;
  //o << "Projector direction: " << m_pProjectorCam->getDirection() << endl;
  //o << "Projector proj: " << m_pProjectorCam->getProjectionMatrix() << endl;
  //o << "Projector view: " << m_pProjectorCam->getViewMatrix() << endl;
  //o << "Render proj: " << m_pRenderCam->getProjectionMatrix() << endl;
  //o << "Render view: " << m_pRenderCam->getViewMatrix() << endl;
  o << "Looking at: " << m_projectorLookAtPoint << endl;
  o << "Range: " << m_rangeMin << " - " << m_rangeMax << endl;
  return o.str();
}

Vector4 PerturbedProjectedGrid::projectHomogeneous(const Ogre::Vector2& point)
{
  // assumes plane normal is +z

  Vector4 from = m_projectionMatrix * Vector4(point.x, point.y, -1, 1);
  Vector4 to   = m_projectionMatrix * Vector4(point.x, point.y, +1, 1);

  Vector4 delta = to - from;
  Real d = m_pSurface->BaseHeight;
  Real t = (from.w * d - from.z) / (delta.z - delta.w * d);

  return from + t * delta;
}