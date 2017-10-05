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

// $(OPENCV_DIR)\lib added as Additional Library References, in Linker --> General
//
// Render a yellow cone and moving using the mouse
// 
/// Class to handle an implementation of an Observer under a Command event
class vtkTimerUser : public vtkCommand
{
  //attributes
private:
  int m_iTimerCount;
  vtkSmartPointer<vtkPoints> m_pPoints;
  int m_iIndex;

public:
  vtkSmartPointer<vtkRenderWindow> render_window;
  // Important!
  static vtkTimerUser *New() 
  {
    vtkTimerUser *pCallBack = new vtkTimerUser;
    pCallBack->m_iTimerCount = 0;
    return pCallBack;
  }

  // Function that execute the main code of the Command
  virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData)) 
  {
    if (vtkCommand::TimerEvent == eventId)
    {
      ++this->m_iTimerCount;
    }
    //if (m_iTimerCount == 1) 
    {
      // get the size of window 
      int windowSize[2];
      render_window->GetInteractor()->GetSize(windowSize);
      // define the filter to get the render
      vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
      windowToImageFilter->SetInput(render_window);
      // using opencv
      cv::Mat image(windowSize[1], windowSize[0], CV_8UC3);
      vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
      imageData = windowToImageFilter->GetOutput();
      windowToImageFilter->Update();

      //windowToImageFilter->GetOutput()->GetScalarPointer()

      image.data = (unsigned char*)imageData->GetScalarPointer();
      const std::string str = "image.jpg";
      cvNamedWindow("ScreenShot", CV_WINDOW_AUTOSIZE);
      cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

      //Mat output(image.size(), CV_8UC3);
      Mat output;
      //image.copyTo(output);
      output = image.clone();
      
      Mat kernel = Mat::ones(5, 5, CV_32F) / (5 * 5);

      //CV_Assert(image.depth() == CV_8U);
      try
      {
        //filter2D(image, output, image.depth(), kernel);
        blur(image, output, Size(7, 7), Point(-1, -1));
      }
      catch (cv::Exception ex) 
      {
        cout << ex.what() << endl;
      }
      //image.release();

      
      //cv::Mat kernel = (cv::Mat_<char>(3, 3) << 0, -1, 0,
      //  -1, 5, -1,
      //  0, -1, 0);
      //cv::filter2D(image, output, image.depth(), kernel);
      
      cv::flip(output, output, cv::ROTATE_90_CLOCKWISE);

      //imshow("ScreenShot", output);
      imshow("ScreenShot", output);
      //cv::imwrite(str, image);
    }
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

  int ASDAD_ASDASDAD = 0;
  int c = ASDAD_ASDASDAD;
  int holaPepe = 0;
  int hola_Pepe = 0;

  // put the geometry of cone inside the pipeline
  vtkSmartPointer<vtkPolyDataMapper> cone_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  cone_mapper->SetInputConnection(cone->GetOutputPort());

  // actor handles properties, color, textures, and others
  vtkSmartPointer<vtkActor> cone_actor = vtkSmartPointer<vtkActor>::New();
  //cone_actor->Print(cout);  //print on console information about cone
  cone_actor->GetProperty()->SetDiffuseColor(0.85, 0.75, 0.1);  //yellow
  cone_actor->SetMapper(cone_mapper);

  // rendered generates the image to be displayed on some place (window/texture)
  vtkSmartPointer<vtkRenderer> rendered = vtkSmartPointer<vtkRenderer>::New();
  rendered->SetBackground(0.2, 0.2, 0.2);
  rendered->AddActor(cone_actor);
  rendered->ResetCamera();  //to be visible to all actors inside scene

  // render window is the GUI
  vtkSmartPointer<vtkRenderWindow> render_window = vtkSmartPointer<vtkRenderWindow>::New();
  render_window->AddRenderer(rendered);
  render_window->SetSize(800, 600);   // viewport' size

  // handling the mouse movements
  vtkSmartPointer<vtkRenderWindowInteractor> render_window_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  render_window_interactor->SetRenderWindow(render_window);

  // Sign up to receive TimerEvent
  //vtkSmartPointer<vtkCommand> cb = vtkSmartPointer<vtkCommand>::New();
  //render_window_interactor->AddObserver(vtkCommand::TimerEvent, cb);
  //vtkSmartPointer<vtkRenderWindowInteractor> qInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  render_window_interactor->Initialize();
  vtkSmartPointer<vtkTimerUser> tCBInstance = vtkSmartPointer<vtkTimerUser>::New();
  tCBInstance->render_window = render_window;
  render_window_interactor->CreateRepeatingTimer(100);
  render_window_interactor->AddObserver(vtkCommand::TimerEvent, tCBInstance);

  //int timerId = render_window_interactor->CreateRepeatingTimer(100);
  //std::cout << "timerId: " << timerId << std::endl;

  render_window->Render();
  render_window_interactor->Start();

  ////// get the size of window 
  ////int windowSize[2];
  ////render_window->GetInteractor()->GetSize(windowSize);
  ////// define the filter to get the render
  ////vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  ////windowToImageFilter->SetInput(render_window);
  ////// using opencv
  ////cv::Mat image(windowSize[1], windowSize[0], CV_8UC3);
  ////vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  ////imageData = windowToImageFilter->GetOutput();
  ////windowToImageFilter->Update();

  ////image.data = (unsigned char*)imageData->GetScalarPointer();
  ////const std::string str = "image.jpg";
  ////cvNamedWindow("ScreenShot", CV_WINDOW_AUTOSIZE);
  ////imshow("ScreenShot", image);
  ////cv::imwrite(str, image);
  //////cvNamedWindow("ScreenShot", CV_WINDOW_AUTOSIZE);
  //////imshow("ScreenShot", image);
  ////cvWaitKey(0);


  //imageData->set
  //imageData->GetScalarPointer( = windowToImageFilter->GetOutput();
  //imageData = windowToImageFilter->GetOutput();
  //windowToImageFilter->GetOutput();

  //image-> .data = (unsigned char*)imageData->GetScalarPointer();
  
  

  // to start the loop and to render 
  
  
  return EXIT_SUCCESS;
}