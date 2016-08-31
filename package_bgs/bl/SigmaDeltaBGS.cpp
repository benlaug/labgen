#include "SigmaDeltaBGS.h"

SigmaDeltaBGS::SigmaDeltaBGS() :
firstTime(true),
ampFactor(1),
minVar(15),
maxVar(255),
algorithm(sdLaMa091New()),
showOutput(false) {

  applyParams();
  std::cout << "SigmaDeltaBGS()" << std::endl;
}

SigmaDeltaBGS::~SigmaDeltaBGS() {
  sdLaMa091Free(algorithm);
  std::cout << "~SigmaDeltaBGS()" << std::endl;
}

void SigmaDeltaBGS::process(
  const cv::Mat &img_input,
  cv::Mat &img_output,
  cv::Mat &img_bgmodel
  ) {
  if (img_input.empty())
    return;

  loadConfig();

  if (firstTime) {
    saveConfig();
    sdLaMa091Init_8u_C3R(algorithm, img_input.data, img_input.cols, img_input.rows, img_input.step);

    firstTime = false;
    return;
  }

  if (img_output.empty())
    img_output = cv::Mat(img_input.rows, img_input.cols, CV_8UC1);

  if (img_bgmodel.empty())
    img_bgmodel = cv::Mat(img_input.rows, img_input.cols, CV_8UC3);

  sdLaMa091Update_8u(algorithm, img_input.data);
  sdLaMa091GetBackgroundModel(algorithm, img_bgmodel.data);
  sdLaMa091Segmentation_8u(algorithm, img_output.data);

  if (showOutput)
    cv::imshow("Sigma-Delta", img_output);
}

void SigmaDeltaBGS::saveConfig() {
  CvFileStorage* fs = cvOpenFileStorage("./config/SigmaDeltaBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "ampFactor", ampFactor);
  cvWriteInt(fs, "minVar", minVar);
  cvWriteInt(fs, "maxVar", maxVar);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void SigmaDeltaBGS::loadConfig() {
  CvFileStorage* fs = cvOpenFileStorage("./config/SigmaDeltaBGS.xml", 0, CV_STORAGE_READ);

  ampFactor = cvReadIntByName(fs, 0, "ampFactor", 1);
  minVar = cvReadIntByName(fs, 0, "minVar", 15);
  maxVar = cvReadIntByName(fs, 0, "maxVar", 255);
  showOutput = cvReadIntByName(fs, 0, "showOutput", false);

  applyParams();

  cvReleaseFileStorage(&fs);
}

void SigmaDeltaBGS::applyParams() {
  sdLaMa091SetAmplificationFactor(algorithm, ampFactor);
  sdLaMa091SetMinimalVariance(algorithm, minVar);
  sdLaMa091SetMaximalVariance(algorithm, maxVar);
}
