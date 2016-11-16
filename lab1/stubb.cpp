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
#include <osg/MatrixTransform>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <string>
#include <iostream>


class MyReadCallback : public osg::NodeCallback {
  
  virtual void operator()(osg::Node* temp, osg::NodeVisitor* nv) {
    //std::cout << "fuckfuck" << std::endl;

    if (intersector_->containsIntersections()) {
        std::cout << intersector_->getFirstIntersection().getWorldIntersectPoint().x() << std::endl;
        std::cout << intersector_->getFirstIntersection().getWorldIntersectPoint().y() << std::endl;
        std::cout << intersector_->getFirstIntersection().getWorldIntersectPoint().z() << std::endl;

    }
    /*if (intersector_->intersects(lodNode_.get()->getBound()))
      std::cout << "hejsomfan" << std::endl;*/
      
  }

  public: 
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector_;
    osg::ref_ptr<osg::LOD> lodNode_;

};


osg::AnimationPath* createAnimationPath(const osg::Vec3& center,float radius,double looptime){

  osg::AnimationPath* animationPath = new osg::AnimationPath;
  animationPath->setLoopMode(osg::AnimationPath::LOOP);

  int numSamples = 40;
  float yaw = 0.0f;
  float yaw_delta = 2.0f*osg::PI/((float)numSamples-1.0f);
  float roll = osg::inDegrees(0.0f);

  double time=0.0f;
  double time_delta = looptime/(double)numSamples;

  for(int i=0;i<numSamples;++i){
    osg::Vec3 position(center+osg::Vec3(sinf(yaw)*radius,cosf(yaw)*radius,0.0f));
    osg::Quat rotation(osg::Quat(roll,osg::Vec3(0.0,1.0,0.0))*osg::Quat(-(yaw+osg::inDegrees(90.0f)),osg::Vec3(0.0,0.0,1.0)));

    animationPath->insert(time,osg::AnimationPath::ControlPoint(position,rotation));
    yaw += yaw_delta;
    time += time_delta;
  }

  return animationPath;
}

osg::Group* createLights(osg::ref_ptr<osg::Group> root) {

  osg::Group* tempGroup = new osg::Group;

  // create a spot light.
  osg::Light* light1 = new osg::Light;
  light1->setLightNum(0);
  light1->setPosition(osg::Vec4(60.0f,-50.0f,-2.0f,1.0f));
  light1->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
  light1->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
  light1->setSpotCutoff(80.0f);
  light1->setSpotExponent(1.0f);
  light1->setDirection(osg::Vec3(0.0f,1.0f,0.0f));

  osg::LightSource* lightS1 = new osg::LightSource;
  lightS1->setLight(light1);
  lightS1->setLocalStateSetModes(osg::StateAttribute::ON);
  lightS1->setStateSetModes(*root->getOrCreateStateSet(),osg::StateAttribute::ON);
  tempGroup->addChild(lightS1);

  osg::Light* light2 = new osg::Light;
  light2->setLightNum(1);
  light2->setPosition(osg::Vec4(-30.0f,-20.0f,0.0f,1.0f));
  light2->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
  light2->setDiffuse(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
  light2->setSpotCutoff(80.0f);
  light2->setSpotExponent(1.0f);
  light2->setDirection(osg::Vec3(0.0f,1.0f,0.0f));

  osg::LightSource* lightS2 = new osg::LightSource;
  lightS2->setLight(light2);
  lightS2->setLocalStateSetModes(osg::StateAttribute::ON);
  lightS2->setStateSetModes(*root->getOrCreateStateSet(),osg::StateAttribute::ON);
  tempGroup->addChild(lightS2);

  return tempGroup;
}

void createGround(osg::ref_ptr<osg::Geode> tempGeode, int tempSize) {

  osg::HeightField* heightField = new osg::HeightField();
  heightField->allocate(tempSize, tempSize);
  heightField->setXInterval(1.0f);
  heightField->setYInterval(1.0f);
  heightField->setSkirtHeight(1.0f);
    
  for (int i = 0; i < tempSize; i++) {
   for (int k = 0; k < tempSize; k++) {
    heightField->setHeight(i, k, 0.5);
   } 
  }

  osg::Image* groundImg = osgDB::readImageFile("ground.jpg");
  osg::Texture2D  *groundTexture = new osg::Texture2D;
  groundTexture->setImage(groundImg);
  tempGeode->addDrawable(new osg::ShapeDrawable(heightField));
  groundTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
  groundTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
  tempGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, groundTexture);
}

osg::ref_ptr<osg::MatrixTransform> createCessnaXform(osg::ref_ptr<osg::LOD> tempLod, osg::ref_ptr<osg::Node> tempCessnaNode, osg::AnimationPath* tempAnimationPath, float radius) {
  
  //Create simplified versions of cessna
  osg::ref_ptr<osg::MatrixTransform> cessnaXform;

  if (tempCessnaNode) {
         const osg::BoundingSphere& bs = tempCessnaNode->getBound();
 
         float size = radius/bs.radius()*0.3f*10.0f;
         osg::MatrixTransform* positioned = new osg::MatrixTransform;
         positioned->setDataVariance(osg::Object::STATIC);
         positioned->setMatrix(osg::Matrix::translate(1.0f, 10.0f, 0.0f)*
                                      osg::Matrix::scale(size,size,size)*
                                      osg::Matrix::rotate(osg::inDegrees(180.0f),0.0f,0.0f,1.0f));
 
         positioned->addChild(tempLod.get());
 
         std::cout << tempLod.get()->getBound().radius()<< std::endl;

         cessnaXform = new osg::MatrixTransform;
         cessnaXform->setUpdateCallback(new osg::AnimationPathCallback(tempAnimationPath,0.0f,2.0));
         cessnaXform->addChild(positioned);
 
  }

  return cessnaXform;
}

osg::ref_ptr<osg::LOD> createLod(osg::ref_ptr<osg::Node> tempCessnaNode) {

  osg::ref_ptr<osg::Node> cessnaNodeLod2 = dynamic_cast<osg::Node*>(tempCessnaNode->clone( osg::CopyOp::DEEP_COPY_ALL));
  osg::ref_ptr<osg::Node> cessnaNodeLod3 = dynamic_cast<osg::Node*>(tempCessnaNode->clone( osg::CopyOp::DEEP_COPY_ALL));

  osgUtil::Simplifier simplifier;

  simplifier.setSampleRatio(0.5);
  cessnaNodeLod2->accept(simplifier);
  
  simplifier.setSampleRatio(0.1);
  cessnaNodeLod3->accept(simplifier);

  osg::ref_ptr<osg::LOD> lodNode = new osg::LOD();
  lodNode->addChild(tempCessnaNode.get(), 0.0f, 50.0f);
  lodNode->addChild(cessnaNodeLod2.get(), 50.0f, 100.0f);
  lodNode->addChild(cessnaNodeLod3.get(), 100.0f, FLT_MAX);

  return lodNode;
}


int main(int argc, char *argv[]){
  
  osg::ref_ptr<osg::Group> root = new osg::Group;

#if 1
  osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode();

  //Tripwire
  osg::Vec3 trip_p0 (-150, 15, -2);
  osg::Vec3 trip_p1 (150, 15, -2);

  osg::ref_ptr<osg::Vec3Array> verticesTrip = new osg::Vec3Array();
  verticesTrip->push_back(trip_p0);
  verticesTrip->push_back(trip_p1);

  osg::ref_ptr<osg::Vec4Array> colorsTrip = new osg::Vec4Array;
  colorsTrip->push_back(osg::Vec4(0.9f,0.2f,0.3f,1.0f));

  osg::ref_ptr<osg::Geometry> linesGeomTrip = new osg::Geometry();
  linesGeomTrip->setVertexArray(verticesTrip);
  linesGeomTrip->setColorArray(colorsTrip, osg::Array::BIND_OVERALL);
  
  linesGeomTrip->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

  
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

  lineGeode->addDrawable(linesGeomTrip);

  /*lineGeode->addDrawable(linesGeomX);
  lineGeode->addDrawable(linesGeomY);
  lineGeode->addDrawable(linesGeomZ);*/

  lineGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

  /// ---
#endif

  
  // Add your stuff to the root node here...

  // Ground ---
  int groundSize = 30;

  //Creates the lights
  osg::Group* lightGroup = createLights(root);

  createGround(lineGeode, groundSize);

  //Creating the animationPath
  osg::Vec3 center(0.0f,0.0f,0.0f);
  float radius = 10.0f;
  float animationLength = 10.0f;
  osg::AnimationPath* animationPath = createAnimationPath(center,radius,animationLength);
  
  osg::ref_ptr<osg::Node> cessnaNode = osgDB::readNodeFile("cessna.osg");
  osg::Node* dumpTruckNode = osgDB::readNodeFile("dumptruck.osg");

  //Creates a level of detail node
  osg::ref_ptr<osg::LOD> lodNode = createLod(cessnaNode);

  //Creates...
  osg::ref_ptr<osg::MatrixTransform> cessnaXform = createCessnaXform(lodNode, cessnaNode, animationPath, radius);

  //Adding intersection functionality
  //osg::ref_ptr<osgUtil::IntersectorGroup> intersectorGroup = new osgUtil::IntersectorGroup();
  osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(trip_p0, trip_p1);
  //intersectorGroup->addIntersector(intersector.get());

  osgUtil::IntersectionVisitor intersectVisitor( intersector.get());

  //intersectVisitor->setUpdateCallback(new IntersectionUpdateCallBack);
  MyReadCallback* myReadCallback = new MyReadCallback;

  myReadCallback->intersector_ = intersector;
  myReadCallback->lodNode_ = lodNode;  

  //cessnaXform->addChild(intersector);

  osg::PositionAttitudeTransform* dumpTruckXform =
   new osg::PositionAttitudeTransform();

  // Declare and initialize a Vec3 instance to change the
  // position of the cessna and dumptruck model in the scene
  /*osg::Vec3 cessnaPos(60.0,0.0,0.0);
  cessnaXform->setPosition(cessnaPos);*/

  osg::Vec3 dumpTruckPos(-70,100.0,0.0);
  dumpTruckXform->setPosition(dumpTruckPos);

  root->addChild(lineGeode);
  root->addChild(cessnaXform);
  root->addChild(dumpTruckXform);
  root->addChild(lightGroup);
  //lineGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, groundTexture);

  
  dumpTruckXform->addChild(dumpTruckNode);


  cessnaXform->accept(intersectVisitor);
  cessnaXform->setUpdateCallback(myReadCallback);
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

  camera->setClearColor(osg::Vec4(0.3f, 0.3f, 0.3f, 1.0f));
  camera->getOrCreateStateSet()->setGlobalDefaults();
  viewer.setCamera(camera);
  
  return viewer.run();
}
