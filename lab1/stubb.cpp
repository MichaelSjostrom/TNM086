#include <osg/Version>
#include <osg/ShapeDrawable>
#include <osg/Image>
#include <osg/Texture2D>

#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>

#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/LightSource>
#include <osg/Light>
#include <osg/Group>
#include <osgUtil/Simplifier>
#include <osg/LOD>



int main(int argc, char *argv[]){
  
  osg::ref_ptr<osg::Group> root = new osg::Group;

#if 1
  osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode();
  
  /// Lines for axis ---
  osg::Vec3 x_p0 (0, 0, 0);
  osg::Vec3 x_p1 (200, 0, 0);
  
  osg::ref_ptr<osg::Vec3Array> verticesX = new osg::Vec3Array();
  verticesX->push_back(x_p0);
  verticesX->push_back(x_p1);

  osg::Vec3 y_p0 ( 0, 0, 0);
  osg::Vec3 y_p1 ( 0, 200, 0);

  osg::ref_ptr<osg::Vec3Array> verticesY = new osg::Vec3Array();
  verticesY->push_back(y_p0);
  verticesY->push_back(y_p1);

  osg::Vec3 z_p0 ( 0, 0, 0);
  osg::Vec3 z_p1 ( 0, 0, 200);

  osg::ref_ptr<osg::Vec3Array> verticesZ = new osg::Vec3Array();
  verticesZ->push_back(z_p0);
  verticesZ->push_back(z_p1);
  
  osg::ref_ptr<osg::Vec4Array> colorsX = new osg::Vec4Array;
  colorsX->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));

  osg::ref_ptr<osg::Vec4Array> colorsY = new osg::Vec4Array;
  colorsY->push_back(osg::Vec4(0.0f,1.0f,0.0f,1.0f));

  osg::ref_ptr<osg::Vec4Array> colorsZ = new osg::Vec4Array;
  colorsZ->push_back(osg::Vec4(0.0f,0.0f,1.0f,1.0f));

  osg::ref_ptr<osg::Geometry> linesGeomX = new osg::Geometry();
  linesGeomX->setVertexArray(verticesX);
  linesGeomX->setColorArray(colorsX, osg::Array::BIND_OVERALL);
  
  linesGeomX->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
  
  osg::ref_ptr<osg::Geometry> linesGeomY = new osg::Geometry();
  linesGeomY->setVertexArray(verticesY);
  linesGeomY->setColorArray(colorsY, osg::Array::BIND_OVERALL);
  
  linesGeomY->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

  osg::ref_ptr<osg::Geometry> linesGeomZ = new osg::Geometry();
  linesGeomZ->setVertexArray(verticesZ);
  linesGeomZ->setColorArray(colorsZ, osg::Array::BIND_OVERALL);
  
  linesGeomZ->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

  /*lineGeode->addDrawable(linesGeomX);
  lineGeode->addDrawable(linesGeomY);
  lineGeode->addDrawable(linesGeomZ);*/

  lineGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

  
  
  // ---


  /// ---
#endif

  
  // Add your stuff to the root node here...

  // Ground ---
  int groundSize = 30;

  osg::Group* lightGroup = new osg::Group;

  // create a spot light.
  osg::Light* light1 = new osg::Light;
  light1->setLightNum(0);
  light1->setPosition(osg::Vec4(60.0f,-20.0f,-2.0f,1.0f));
  light1->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
  light1->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
  light1->setSpotCutoff(80.0f);
  light1->setSpotExponent(1.0f);
  light1->setDirection(osg::Vec3(0.0f,1.0f,0.0f));

  osg::LightSource* lightS1 = new osg::LightSource;
  lightS1->setLight(light1);
  lightS1->setLocalStateSetModes(osg::StateAttribute::ON);
  lightS1->setStateSetModes(*root->getOrCreateStateSet(),osg::StateAttribute::ON);
  lightGroup->addChild(lightS1);

  osg::Light* light2 = new osg::Light;
  light2->setLightNum(1);
  light2->setPosition(osg::Vec4(-30.0f,-20.0f,0.0f,1.0f));
  light2->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
  light2->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
  light2->setSpotCutoff(80.0f);
  light2->setSpotExponent(1.0f);
  light2->setDirection(osg::Vec3(0.0f,1.0f,0.0f));

  osg::LightSource* lightS2 = new osg::LightSource;
  lightS2->setLight(light2);
  lightS2->setLocalStateSetModes(osg::StateAttribute::ON);
  lightS2->setStateSetModes(*root->getOrCreateStateSet(),osg::StateAttribute::ON);
  lightGroup->addChild(lightS2);


  osg::HeightField* heightField = new osg::HeightField();
  heightField->allocate(groundSize, groundSize);
  heightField->setXInterval(1.0f);
  heightField->setYInterval(1.0f);
  heightField->setSkirtHeight(1.0f);
    
  for (int i = 0; i < groundSize; i++) {
   for (int k = 0; k < groundSize; k++) {
    heightField->setHeight(i, k, 0.5);
   } 
  }

  osg::Image* groundImg = osgDB::readImageFile("ground.jpg");
  osg::Texture2D  *groundTexture = new osg::Texture2D;
  groundTexture->setImage(groundImg);
  lineGeode->addDrawable(new osg::ShapeDrawable(heightField));
  groundTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
  groundTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
  lineGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, groundTexture);
  
  osg::ref_ptr<osg::Node> cessnaNode = osgDB::readNodeFile("cessna.osg");
  osg::Node* dumpTruckNode = osgDB::readNodeFile("dumptruck.osg");

  osg::PositionAttitudeTransform* cessnaXform =
   new osg::PositionAttitudeTransform();

  osg::PositionAttitudeTransform* dumpTruckXform =
   new osg::PositionAttitudeTransform();

  // Declare and initialize a Vec3 instance to change the
  // position of the cessna and dumptruck model in the scene
  osg::Vec3 cessnaPos(60.0,0.0,0.0);
  cessnaXform->setPosition(cessnaPos);

  osg::Vec3 dumpTruckPos(-70,0.0,0.0);
  dumpTruckXform->setPosition(dumpTruckPos);

  //Create simplified versions of cessna
  osg::ref_ptr<osg::Node> cessnaNodeLod2 = dynamic_cast<osg::Node*>(cessnaNode->clone( osg::CopyOp::DEEP_COPY_ALL));
  osg::ref_ptr<osg::Node> cessnaNodeLod3 = dynamic_cast<osg::Node*>(cessnaNode->clone( osg::CopyOp::DEEP_COPY_ALL));

  osgUtil::Simplifier simplifier;

  simplifier.setSampleRatio(0.5);
  cessnaNodeLod2->accept(simplifier);
  
  simplifier.setSampleRatio(0.1);
  cessnaNodeLod3->accept(simplifier);

  osg::ref_ptr<osg::LOD> lodNode = new osg::LOD();
  lodNode->addChild(cessnaNode.get(), 0.0f, 10.0f);
  lodNode->addChild(cessnaNodeLod2.get(), 10.0f, 30.0f);
  lodNode->addChild(cessnaNodeLod3.get(), 30.0f, FLT_MAX);
   

  root->addChild(lineGeode);
  root->addChild(cessnaXform);
  root->addChild(dumpTruckXform);
  root->addChild(lightGroup);
  lineGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, groundTexture);

  
  dumpTruckXform->addChild(dumpTruckNode);
  cessnaXform->addChild(lodNode.get());

  // Optimizes the scene-graph
  //osgUtil::Optimizer optimizer;
  //optimizer.optimize(root);
  
  // Set up the viewer and add the scene-graph root
  osgViewer::Viewer viewer;
  viewer.setSceneData(root);

  osg::ref_ptr<osg::Camera> camera = new osg::Camera;
  camera->setProjectionMatrixAsPerspective(60.0, 1.0, 0.1, 100.0);
  camera->setViewMatrixAsLookAt (osg::Vec3d(0.0, 0.0, 2.0),
                                 osg::Vec3d(0.0, 0.0, 0.0),
                                 osg::Vec3d(0.0, 1.0, 0.0));
  camera->getOrCreateStateSet()->setGlobalDefaults();
  viewer.setCamera(camera);
  
  return viewer.run();
}
