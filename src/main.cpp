#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkWindows.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkCommand.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

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

  // Important! the constructor
  static vtkTimerUser *New()
  {
    vtkTimerUser *pCallBack = new vtkTimerUser;
    return pCallBack;
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


int main (int argc, char *argv[])
{
  // polygonal model of a cone
  vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetResolution(50);
  cone->SetHeight(4.0);
  cone->SetRadius(1.0);

  // put the geometry of cone inside the pipeline
  vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  coneMapper->SetInputConnection(cone->GetOutputPort());

  // actor handles properties, color, textures, and others
  vtkSmartPointer<vtkActor> coneActor = vtkSmartPointer<vtkActor>::New();
  //cone_actor->Print(cout);  //print on console information about cone
  coneActor->GetProperty()->SetDiffuseColor(0.85, 0.75, 0.1);  //yellow
  coneActor->SetMapper(coneMapper);

  // rendered generates the image to be displayed on some place (window/texture)
  vtkSmartPointer<vtkRenderer> rendered = vtkSmartPointer<vtkRenderer>::New();
  rendered->SetBackground(0.2, 0.2, 0.2);
  rendered->AddActor(coneActor);
  rendered->ResetCamera();  //to be visible to all actors inside scene

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