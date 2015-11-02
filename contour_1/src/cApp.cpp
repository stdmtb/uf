#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"
#include "cinder/params/Params.h"
#include "CinderOpenCv.h"

#include "ufUtil.h"
#include "Exporter.h"
#include "MyCamera.h"
#include "ContourMap.h"

//#define RENDER

using namespace ci;
using namespace ci::app;
using namespace std;

class cApp : public AppNative {
    
public:
    void setup();
    void keyDown( KeyEvent event );
    void update();
    void draw();
  	void resize();
    
    int mWin_w = 1920;
    int mWin_h = 1080;
    int n_threshold;

    gl::Texture mTex;
    params::InterfaceGlRef mParams;
    Quatf mObjOrientation;
    Perlin mPln;
    vector<ContourMap> mCMaps;
    MyCamera mCam;
    Exporter mExp;
};

void cApp::setup(){
    
    mPln.setSeed( 123 );
    mPln.setOctaves( 3 );
    
    n_threshold = 0;
    setWindowPos( 0, 0 );
    setWindowSize( mWin_w, mWin_h );
    mExp.setup( mWin_w, mWin_h, 3001, GL_RGB, uf::getRenderPath(), 0 );
    
    // load image
    vector<Surface32f> surs;
//    surs.push_back( Surface32f( loadImage((loadAsset("vela_orient_blue_pac70_signal.tiff")))) );
    surs.push_back( Surface32f( loadImage((loadAsset("img/01/1.tif")))) );
    surs.push_back( Surface32f( loadImage((loadAsset("img/01/2.tif")))) );
    surs.push_back( Surface32f( loadImage((loadAsset("img/01/3.tif")))) );
    surs.push_back( Surface32f( loadImage((loadAsset("img/01/4.tif")))) );
    surs.push_back( Surface32f( loadImage((loadAsset("img/01/vela_scana_spire350_signal.tiff")))) );

    for ( auto & s : surs ) {
        ContourMap cm;
        cm.setImage( s, true, cv::Size(1,1) );
        mCMaps.push_back( cm );
    }
    
    mCMaps[0].addContour( 0.05, randInt(0,3) );
    mCMaps[0].addContour( 0.08, randInt(0,3) );
    mCMaps[0].addContour( 0.10, randInt(0,3) );
    mCMaps[0].addContour( 0.13, randInt(0,3) );
    mCMaps[0].addContour( 0.17, randInt(0,3) );
    
    mCMaps[1].addContour( 0.1 , randInt(0,3));
    mCMaps[1].addContour( 0.14, randInt(0,3) );
    mCMaps[1].addContour( 0.18, randInt(0,3) );
    mCMaps[1].addContour( 0.21, randInt(0,3) );
    mCMaps[1].addContour( 0.25, randInt(0,3) );

    mCMaps[2].addContour( 0.1 , randInt(0,3) );
    mCMaps[2].addContour( 0.12, randInt(0,3) );
    mCMaps[2].addContour( 0.14, randInt(0,3) );
    mCMaps[2].addContour( 0.16, randInt(0,3) );
    mCMaps[2].addContour( 0.19, randInt(0,3) );

    mCMaps[3].addContour( 0.12, randInt(0,3) );
    mCMaps[3].addContour( 0.13, randInt(0,3) );
    mCMaps[3].addContour( 0.14, randInt(0,3) );
    mCMaps[3].addContour( 0.16, randInt(0,3) );
    mCMaps[3].addContour( 0.18, randInt(0,3) );
    
    mCMaps[4].addContour( 0.10, randInt(0,3) );
    mCMaps[4].addContour( 0.15, randInt(0,3) );
    mCMaps[4].addContour( 0.20, randInt(0,3) );
    mCMaps[4].addContour( 0.23, randInt(0,3) );
    mCMaps[4].addContour( 0.30, randInt(0,3) );
    mCMaps[4].addContour( 0.33, randInt(0,3) );
    mCMaps[4].addContour( 0.40, randInt(0,3) );

    
    surs.clear();

    // Camera
    mCam.setFov( 54 );
   	mCam.lookAt( Vec3f( 0, 0, 4200 ), Vec3f::zero() );
    mCam.setNearClip(1);
    mCam.setFarClip(100000);

    mCam.setup();
    
    // Interface
    mParams = params::InterfaceGl::create("params", toPixels( Vec2i(250, 250) ) );
    mParams->setOptions ( "", "position=`210 10` valueswidth=150" );
    mParams->addParam("rotation", &mObjOrientation);
    
#ifdef RENDER
    mExp.startRender();
#endif
}

void cApp::update(){
}

void cApp::draw(){
    gl::clear();
    gl::enableAlphaBlending();
    mExp.begin(); {
        gl::clear();
        gl::setMatrices( mCam );
        gl::pushMatrices();
        gl::rotate( mObjOrientation );
        gl::translate( -5200, 0 ,0 );
        
        int mapId = -1;

//        for( auto & map : mCMaps ){
//            mapId++;
//            gl::translate( 1500, 0, 0 );
//            for( int i=0; i<map.mCMapData.size(); i++ ){
//                gl::pushMatrices();
//                gl::translate( 0, 0, i*3 );
//                gl::color(mapId*0.2, 0.5+i*0.02, i*0.05);
//                map.drawContourGroup(i);
//                gl::popMatrices();
//            }
//        }
        
#define SCAN
#ifdef SCAN
        float frame = getElapsedFrames();
        float scanSpeed = 5;
        
        for( auto & map : mCMaps ){
            mapId++;
            gl::translate( 1480, 0, 0 );
            for( int i=0; i<map.mCMapData.size(); i++ ){
                
                ContourMap::ContourGroup &cg = map.mCMapData[i];
                int nVertex = 0;
                int totalVerts = 0;
                Vec2f scanPoint;
                bool scanFinish = false;
                for( int j=0; j<cg.size(); j++ ){
                    totalVerts += cg[j].size();
                }
                
                for( int j=0; j<cg.size(); j++ ){

                    ContourMap::Contour & c = cg[j];

                    glPointSize(1);
                    glBegin( GL_POINTS );
                    for( int k=0; k<c.size(); k++ ){
                        scanFinish = (j==cg.size()-1) && (k==c.size()-1);
                        
                        if( ++nVertex < frame*scanSpeed){
                            cv::Point2f & p = c[k];
                            glColor4f( 0.2+i*0.2+mPln.noise(j)*0.1, 0.8-i*0.1+mapId*0.01, i*0.1+j*0.01+mPln.noise(i)*0.02, k*0.06 );
                            gl::vertex( fromOcv(p) );
                            if( !scanFinish ){
                                scanPoint = fromOcv(p);
                            }
                            
                            if( frame > 300 ){
                                p.x += mPln.fBm( mapId*0.5, i*0.5, frame*0.005 )*7.0;
                                p.y += mPln.fBm( mapId*0.5, j*0.5, frame*0.005 )*5.0;
                            }
                        }else{
                            break;
                        }
                    }
                    glEnd();
                }
                
                if( scanFinish ){
                    int scanId = (int)(frame*scanSpeed) % totalVerts;
                    //scanPoint = fromOcv( *cg[0][scanId] );
                }
            
                //if( !scanFinish ){
                    glLineWidth( 1 );
                    glBegin( GL_LINES );
                    gl::vertex( scanPoint );
                    scanPoint.y = -10000;
                    gl::vertex( scanPoint );
                    glEnd();
                //}
            }
        }
#endif
        gl::popMatrices();
        
    } mExp.end();
    
    gl::color( Colorf::white() );
    mExp.draw();

    uf::drawScreenGuide();
    mParams->draw();
    mCam.drawParam();
}

void cApp::keyDown( KeyEvent event ) {
    char key = event.getChar();
    switch (key) {
        case 'S':
            mExp.startRender();
            break;
        case 'T':
            mExp.stopRender();
            break;
            
        case 't':
            n_threshold++;
            n_threshold %= 8;
            break;
            
        case 'e':
            string epsPath = "../../out/eps/";
            createDirectories( epsPath );
            mCMaps[0].exportContour( epsPath+"contour", "eps" );
            break;
        }
}

void cApp::resize(){
    mCam.setAspectRatio( getWindowAspectRatio() );
}

CINDER_APP_NATIVE( cApp, RendererGl(0) )