#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkProgressObserver.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkOBJReader.h>
#include <vtkRendererCollection.h>
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

//
// Render a yellow cone and moving using the mouse.
// Also copying the render is displayed in a OpenCV window with
// a blurring effect
// 

/// Class to handle an implementation of an Observer under a Command event
class vtkTimerUser : public vtkCommand
{
public:
  vtkSmartPointer<vtkRenderWindow> m_renderWindow; //to be copied from VTK instance
  vtkSmartPointer<vtkPoints> m_pPoints;   // store the path
  int m_iIndex;   // current index into the path

  // Important! the constructor
  static vtkTimerUser *New()
  {
    vtkTimerUser *pCallBack = new vtkTimerUser;
    return pCallBack;
  }

  void vtkTimerUser::setPath(const vtkSmartPointer<vtkPoints> & pPoints)
  {
    m_pPoints = pPoints;
    m_iIndex = 0;
  }

  // Function that execute the main code of the Command
  virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData))
  {
    // Get the size of window 
    int windowSize[2];
    m_renderWindow->GetInteractor()->GetSize(windowSize);

    // Define the filter to extract the image
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(m_renderWindow);

    // Copy image from VTK to OPENCV
    cv::Mat matImage(windowSize[1], windowSize[0], CV_8UC3); // 8 bits x unsigned char x 3 channels
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData = windowToImageFilter->GetOutput(); 
    windowToImageFilter->Update();

    matImage.data = (unsigned char*)imageData->GetScalarPointer();
    
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
    const std::string str = "image.jpg";
    cv::imwrite(str, matImage);
    
    // Safe normally to continue
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
    iren->GetRenderWindow()->Render();
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

  navigation.computePath(98);

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


  // Broncho actor
  vtkSmartPointer<vtkActor> volumeActor = vtkSmartPointer<vtkActor>::New();
  volumeActor->GetProperty()->SetDiffuseColor(0.90, 0.40, 0.25);  //red
  volumeActor->SetMapper(volMapper);

  // actor handles properties, color, textures, and others
  vtkSmartPointer<vtkActor> pointActor = vtkSmartPointer<vtkActor>::New();
  //cone_actor->Print(cout);  //print on console information about cone
  pointActor->GetProperty()->SetDiffuseColor(0.85, 0.75, 0.1);  //yellow
  pointActor->SetMapper(pointMapper);

  // rendered generates the image to be displayed on some place (window/texture)
  vtkSmartPointer<vtkRenderer> rendered = vtkSmartPointer<vtkRenderer>::New();
  rendered->SetBackground(0.2, 0.2, 0.2);
  rendered->AddActor(pointActor);
  rendered->AddActor(volumeActor);
  // Set the camera
  vtkSmartPointer<vtkCamera> tCamera = rendered->GetActiveCamera();
  tCamera->SetPosition(242.29, 203.07, 488.58);
  tCamera->SetFocalPoint(223.96, 202.18, 327.66);
  //rendered->ResetCamera();  //to be visible to all actors inside scene

  // render window is the GUI
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(rendered);
  renderWindow->SetSize(800, 600);   // viewport' size

  // handling the mouse movements
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Sign up to receive TimerEvent
  renderWindowInteractor->Initialize();
  vtkSmartPointer<vtkTimerUser> tCBInstance = vtkSmartPointer<vtkTimerUser>::New();
 // Copy important values to be used on the class
  tCBInstance->m_renderWindow = renderWindow;
  renderWindowInteractor->CreateRepeatingTimer(100);  //100 milliseconds
  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, tCBInstance);

  // to start the loop and to render 
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}