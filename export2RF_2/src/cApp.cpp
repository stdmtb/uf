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
#include "ConsoleColor.h"
#include "DataGroup.h"
#include "RfExporterBin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class cApp : public AppNative {
    
public:
    void setup();
    void update();
    void draw();
    void mouseDown( MouseEvent event );
    void mouseDrag( MouseEvent event );
    void keyDown( KeyEvent event );
    void resize();
    
    MayaCamUI camUi;
    Perlin mPln;
    
    DataGroup mDg;
};

void cApp::setup(){
    setWindowPos( 0, 0 );
    setWindowSize( 1920, 1080 );
    
    CameraPersp cam( 1920, 1080, 54.4f, 1, 100000 );
    cam.lookAt( Vec3f(0,0,1000), Vec3f(0,0,0) );
    cam.setCenterOfInterestPoint( Vec3f(0,0,0) );
    camUi.setCurrentCam( cam );
    
    mPln.setSeed(123);
    mPln.setOctaves(4);
    
    
    vector<Vec3f> ps;
    vector<ColorAf> cs;
    vector<float> pos;
    vector<float> vel;

    
    {
        // make point from intensity
        Surface32f sIntensity( loadImage(loadAsset("img/02/vela_scana_spire500_signal.tiff")) );
        int intensityW = sIntensity.getWidth();
        int intensityH = sIntensity.getHeight();
        
        float threashold = 0.4;
        
        Surface32f::Iter itr = sIntensity.getIter();
        while ( itr.line() ) {
            while( itr.pixel() ){
                float gray = itr.r();
                
                if( threashold<gray && gray<0.7 ){
                    Vec2i posi = itr.getPos();
                    float extrusion = 100.0f;
                    Vec3f v(posi.x-intensityW/2, posi.y-intensityH/2, gray*extrusion );
                    Vec3f noise = mPln.dfBm( Vec3f(posi.x, posi.y, gray) );
                    
                    v *= 0.1;
                    
                    ps.push_back( v + noise );

                    ColorAf c( randFloat(), gray, randFloat(), 1);
                    cs.push_back( c );
                    
                    pos.push_back( v.x );
                    pos.push_back( v.y );
                    pos.push_back( v.z );
                    
                    vel.push_back( c.r-0.5 );
                    vel.push_back( c.g-0.5 );
                    vel.push_back( -c.r );
                }
            }
        }
    }
    
    mDg.createDot( ps, cs, 0.0 );
    RfExporterBin rfB;
    rfB.write( /* uf::getTimeStamp() +*/ "myParticle_00000.bin", pos, vel );
}

void cApp::update(){
}

void cApp::draw(){
    
    gl::clear( ColorA(1,1,1,1) );

    gl::setMatrices( camUi.getCamera() );
    uf::drawCoordinate( 10 );
    
    // data
    glPointSize( 1 );
    gl::draw( mDg.mDot );
}

void cApp::keyDown( KeyEvent event ) {
    char key = event.getChar();
    switch (key) {
        case 'E':
            
            break;
    }
}

void cApp::mouseDown( MouseEvent event ){
    camUi.mouseDown( event.getPos() );
}

void cApp::mouseDrag( MouseEvent event ){
    camUi.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void cApp::resize(){
}

CINDER_APP_NATIVE( cApp, RendererGl(0) )
