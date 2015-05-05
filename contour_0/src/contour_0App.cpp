#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include "CinderOpenCv.h"
#include "cinder/cairo/Cairo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class contour_0App : public AppNative {
  public:
	void setup();
    void keyDown( KeyEvent event );

    void mouseMove( MouseEvent event );
    void mouseDown( MouseEvent event );
    void mouseDrag( MouseEvent event );
    void update();
	void draw();
    void resize();

    Vec2f mMousePos;
    MayaCamUI mMayaCam;

    cv::Mat input;
    cv::Mat thresh;
    typedef vector<cv::Point> Contour;
    typedef vector<Contour> ContourGroup;
    ContourGroup cg;
    
    vector<cv::Vec4i> hierarchy;
};

void contour_0App::setup(){
    setWindowSize(1920, 1080);
    setWindowPos(0, 0);
    
    CameraPersp cam;
    cam.setEyePoint( Vec3f(0, 0, 2000.0f) );
    cam.setCenterOfInterestPoint( Vec3f(0, 0, 0.0) );
    cam.setPerspective( 60.0f, getWindowAspectRatio(), 1.0f, 10000.0f );
    mMayaCam.setCurrentCam( cam );
    
    float threshlod = 0.1;
    
    // CONTOUR
    Surface32f sur( loadImage((loadAsset("1.tif"))) );
    gl::Texture mTex = gl::Texture( sur );
    input = toOcv( sur );
    cv::cvtColor( input, input, CV_RGB2GRAY );
    cv::blur( input, input, cv::Size(2,2) );
    cv::threshold( input, thresh, threshlod, 1.0, CV_THRESH_BINARY );

    cv::Mat thresh32sc1;
    thresh.convertTo(thresh32sc1, CV_32SC1 );
    cv::findContours( thresh32sc1.clone(), cg, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
    
    // export thresh image
    //Surface32f thresh_sur = fromOcv( thresh );
    //writeImage( "../../../out/" + (toString(threshlod)+".png"), thresh_sur );
    
    // Export eps
    cairo::Context ctx;
    ctx = cairo::Context( cairo::SurfaceEps( fs::path("../../../out/tests.eps"), 3000, 3000 ) );
    ctx.setSource( ColorAf(1,1,1,1) );
    ctx.paint();
    ctx.stroke();
    
    ctx.setLineWidth(1);
    for( int i=0; i<cg.size(); i++){
      //if( i%2==1)  continue;
        bool haveParent = hierarchy[i][2] < 0;
        bool haveChild = hierarchy[i][3] < 0;
        
        if( haveParent && haveChild )
            ctx.setSource( Colorf(0.5,0.5,0.5) );
        else if( haveParent && !haveChild ){
            ctx.setSource( Colorf(1,0,0) );
            //continue;
        }else if( !haveParent && haveChild ){
            ctx.setSource( Colorf(0,0,1) );
            //continue;
        }else if( !haveParent && !haveChild ){
            ctx.setSource( Colorf(0,0,0) );
        }else {
            ctx.setSource( Colorf(1,0,0.3) );
        }
            
        ctx.newPath();
        for( auto & p : cg[i] ){
            ctx.lineTo( p.x, p.y );
        }
        ctx.closePath();
        ctx.stroke();
    }

    
}



void contour_0App::update(){
}

void contour_0App::draw(){
    
    gl::pushMatrices();
    gl::setMatrices( mMayaCam.getCamera() );
    gl::clear( Color(1,1,1) );
    gl::color( Color( 0,0,0) );
    gl::drawStrokedCube(Vec3f(0,0,0), Vec3f(100,100,100) );
    
    
    glBegin( GL_POINTS );
    for( int i=0; i<cg.size(); i++ ){
        Contour & c = cg[i];
        
        if( hierarchy[i][2] == -1 ){
            gl::color(1, 0, 0);
        }else if( hierarchy[i][3] == -1 ){
            gl::color(0, 1, 0);
        }
        
        for( int j=0; j<c.size(); j++){
            gl::vertex( fromOcv(c[j]) );
        }
    }
    
    glEnd();

    
    
    gl::popMatrices();

}

void contour_0App::keyDown( KeyEvent event ){
    
}

void contour_0App::mouseDown( MouseEvent event ){
    mMayaCam.mouseDown( event.getPos() );
}

void contour_0App::mouseMove( MouseEvent event ){
    mMousePos = event.getPos();
}

void contour_0App::mouseDrag( MouseEvent event ){
    mMousePos = event.getPos();
    mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}


void contour_0App::resize(){
    CameraPersp cam = mMayaCam.getCamera();
    cam.setAspectRatio( getWindowAspectRatio() );
    mMayaCam.setCurrentCam( cam );
}

CINDER_APP_NATIVE( contour_0App, RendererGl )
