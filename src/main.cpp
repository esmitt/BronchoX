#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkPropPicker.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkProgressObserver.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkOBJReader.h>
#include <vtkRendererCollection.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkWindows.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkCommand.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>

#include <vtkTriangleFilter.h>
#include <vtkLoopSubdivisionFilter.h>

#include "graph.h"
#include "navigation.h"

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <fstream>

using namespace cv;
using namespace vtk;

const int startIndex = 4657;


class vtkLeftClicking : public vtkCallbackCommand 
//class vtkLeftClicking : public vtkInteractorStyleTrackballCamera
{
public:
  Graph* graph;
  CNavigation* navigation;
  vtkSmartPointer<vtkRenderer> rendered;  //to add the actor

  static vtkLeftClicking *New()
  {
    vtkLeftClicking *pCallBack = new vtkLeftClicking;
    return pCallBack;
  }
  virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData))
  {
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (interactor == nullptr) return;

    auto pick = interactor->GetPicker();
    if (pick->GetPickList() == nullptr) return;

    auto a = pick->GetPickList();
    cout << a->GetNumberOfItems() << endl;

    double* p = pick->GetPickPosition();
    cout << p[0] << " " << p[1] << " " << p[2] << endl;	//x, y, z
    int index = graph->getID(p[0], p[1], p[2]);
    if (index >= 0 && p[0] != 0) 
    {
      cout << "index: " << index << endl;
      //add actor
      vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      //mapper->SetInputData(navigation->getDrawingPath(startIndex, index, "adsad"));
      mapper->SetInputData(navigation->getSmoothPath(startIndex, index));
      cout << "asdadsadasdasda " << endl;
      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
      actor->SetMapper(mapper);
      rendered->AddActor(actor);
    }
  }
};
//
// Render a yellow cone and moving using the mouse.
// Also copying the render is displayed in a OpenCV window with
// a blurring effect
// 

/// Class to handle an implementation of an Observer under a Command event
class vtkTimerUser : public vtkCallbackCommand
{
public:
  //vtkSmartPointer<vtkRenderWindow> m_renderWindow; //to be copied from VTK instance
  vtkSmartPointer<vtkPoints> m_pPoints;   // store the path
  int m_iIndex;   // current index into the path

  // Important! the constructor
  static vtkTimerUser *New()
  {
    //vtkTimerUser *pCallBack = new vtkTimerUser;
    //return pCallBack;
    return new vtkTimerUser;
  }

  void vtkTimerUser::setPath(const vtkSmartPointer<vtkPoints> & pPoints)
  {
    m_pPoints = pPoints;
    m_iIndex = 0;
  }

  void OpenCVProcessing(const vtkSmartPointer<vtkRenderWindow> renderWindow, int w, int h)
  {
    // Define the filter to extract the image
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    //windowToImageFilter->SetInput(m_renderWindow);
    windowToImageFilter->SetInput(renderWindow);

    // Copy image from VTK to OPENCV
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData = windowToImageFilter->GetOutput();
    windowToImageFilter->Update();

    // Pointer array directly
    cv::Mat matImage(w, h, CV_8UC3, imageData->GetScalarPointer()); // 8 bits x unsigned char x 3 channels
    cv::cvtColor(matImage, matImage, cv::COLOR_BGR2RGB);    // change from BGR --> RGB
    cv::flip(matImage, matImage, cv::ROTATE_90_CLOCKWISE);  // original image is turn down

    // Just in case, add some try/catch about operations in OpenCV
    try
    {
      cv::blur(matImage, matImage, Size(7, 7), Point(-1, -1));
    }
    catch (cv::Exception ex)
    {
      cout << ex.what() << endl;
    }
    imshow("OpenCV", matImage); // show the result

    // It is possible to write the output image on a file
    //const std::string str = "image.jpg";
    //cv::imwrite(str, matImage);
  }

  // Function that execute the main code of the Command
  virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData))
  {
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (interactor == nullptr) return;


    //int* clickPos = interactor->GetEventPosition();
    //vtkSmartPointer<vtkPropPicker> picker = vtkSmartPointer<vtkPropPicker>::New();
    //interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()
    //picker->Pick(clickPos[0], clickPos[1], 0, interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
    //// Get the current position
    //if (m_pPoints != nullptr)
    //{
    //  if (m_iIndex == 0)  //first point
    //  {
    //    double* point = new double[3];
    //    //set the initial camera's position
    //    point = m_pPoints->GetPoint(0);	//the 1st
    //    //m_vtkCamera->SetPosition(point);
    //  }
    //  else 
    //  {
    //  
    //  }
    //}

    // Get the size of window & invoke the OpenCv calculations
    int windowSize[2];
    interactor->GetSize(windowSize);
    OpenCVProcessing(interactor->GetRenderWindow(), windowSize[1], windowSize[0]);
    
    // Safe normally to continue
    //vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
    interactor->Render();
    //iren->GetRenderWindow()->Render();
  }
};

// Just for testing, a Loop subdivision algorithm to the input OBJ
// the output is the subdivide mesh
vtkSmartPointer<vtkPolyData> Subdivide(vtkSmartPointer<vtkPolyData> reader)
{
  // Subdivision filters only work on triangles
  vtkSmartPointer<vtkTriangleFilter> triangles = vtkSmartPointer<vtkTriangleFilter>::New();
  //triangles->SetInputConnection(reader-> ->GetOutputPort());
  triangles->SetInputData(reader);
  triangles->Update();
  vtkSmartPointer<vtkPolyData> originalMesh = triangles->GetOutput();  // this is the original mesh actually

  vtkSmartPointer<vtkPolyDataAlgorithm> subdivisionFilter = vtkSmartPointer<vtkLoopSubdivisionFilter >::New();
  dynamic_cast<vtkLoopSubdivisionFilter *> (subdivisionFilter.GetPointer())->SetNumberOfSubdivisions(2);
  subdivisionFilter->SetInputData(originalMesh);
  subdivisionFilter->Update();

  return subdivisionFilter->GetOutput();
}

int main (int argc, char *argv[])
{
  CNavigation navigation;
  // Object to handle a path in the volume
  //Graph theData;

  // Read the OBJ bronchi
  //std::string filename = "Q:\\Experiments\\CTBronquiSeg\\CPAP\\LENS_P15_13_11_2015\\LENS_P15_13_11_2015_ESP_CPAP_Seg1.obj"; //relative path
  std::string filename = "C:\\Users\\eramirez\\Desktop\\code\\bronchi labelling\\output.obj";
  vtkSmartPointer<vtkOBJReader> readerOBJ = vtkSmartPointer<vtkOBJReader>::New();
  readerOBJ->SetFileName(filename.c_str());
  readerOBJ->Update();
  //readerOBJ->Print(cout);

  if (!navigation.openTXTFile("C:\\Users\\eramirez\\Desktop\\git\\BronchoX\\skel.txt"))
    return EXIT_FAILURE;

  // Set Navigation

  //navigation.computePath(98);
  navigation.computePath(startIndex);
  //navigation.computeMST(startIndex);
  

  // Mapper
  // lines
  vtkSmartPointer<vtkPolyDataMapper> linesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //linesMapper->SetInputData(navigation.getDrawingPath(98, 2934, "blabla"));
  //linesMapper->SetInputData(navigation.getSmoothPath(startIndex, 2934));

  // put the geometry of volume inside the pipeline
  vtkSmartPointer<vtkPolyDataMapper> volMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //auto outputGeometry = Subdivide(readerOBJ->GetOutput());
  //volMapper->SetInputData(outputGeometry);
  volMapper->SetInputConnection(readerOBJ->GetOutputPort());

  // put the geometry of points inside the pipeline
  vtkSmartPointer<vtkPolyDataMapper> pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
 ///////// pointMapper->SetInputData(theData.getDrawablePoints());
  //pointMapper->SetInputData(navigation.getDrawablePoints());
  pointMapper->SetInputData(navigation.getDrawablePoints());

  // Actor
  // Broncho actor
  vtkSmartPointer<vtkActor> volumeActor = vtkSmartPointer<vtkActor>::New();
  volumeActor->GetProperty()->SetDiffuseColor(0.90, 0.40, 0.25);  //red
  volumeActor->SetMapper(volMapper);

  // actor handles properties, color, textures, and others
  vtkSmartPointer<vtkActor> pointActor = vtkSmartPointer<vtkActor>::New();
  //cone_actor->Print(cout);  //print on console information about cone
  pointActor->GetProperty()->SetDiffuseColor(0.85, 0.75, 0.1);  //yellow
  pointActor->SetMapper(pointMapper);

  //lines actor
  vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
  lineActor->GetProperty()->SetDiffuseColor(0.05, 0.75, 0.85);  //green
  //lineActor->SetMapper(linesMapper);

  // rendered generates the image to be displayed on some place (window/texture)
  vtkSmartPointer<vtkRenderer> rendered = vtkSmartPointer<vtkRenderer>::New();
  rendered->SetBackground(0.2, 0.2, 0.2);
  rendered->AddActor(pointActor);
  //rendered->AddActor(volumeActor);
  rendered->AddActor(lineActor);
  // Set the camera
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> cameraStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  //vtkSmartPointer<vtkCamera> tCamera = rendered->GetActiveCamera();
  vtkSmartPointer<vtkCamera> tCamera = vtkSmartPointer<vtkCamera>::New();
  tCamera->SetPosition(242.29, 203.07, 488.58);
  tCamera->SetFocalPoint(223.96, 202.18, 327.66);
  rendered->SetActiveCamera(tCamera);
  
  //rendered->ResetCamera();  //to be visible to all actors inside scene

  // render window is the GUI
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(800, 600);   // viewport' size
  renderWindow->SetWindowName("BronchoX");
  renderWindow->AddRenderer(rendered);

  // handling the mouse movements
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->SetInteractorStyle(cameraStyle);

  // Sign up to receive TimerEvent
  renderWindowInteractor->Initialize();
  renderWindowInteractor->CreateRepeatingTimer(100);  //100 milliseconds

  vtkSmartPointer<vtkTimerUser> tCBInstance = vtkSmartPointer<vtkTimerUser>::New();
  vtkSmartPointer<vtkLeftClicking> leftCBInstance = vtkSmartPointer<vtkLeftClicking>::New();
  leftCBInstance->navigation = &navigation;
  leftCBInstance->graph = navigation.getGraph();
  leftCBInstance->rendered = rendered;
 // Copy important values to be used on the class
  //tCBInstance->m_renderWindow = renderWindow;
  
  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, tCBInstance);
  renderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, leftCBInstance);

  // to start the loop and to render 
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}