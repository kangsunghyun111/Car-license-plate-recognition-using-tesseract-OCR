# Car-license-plate-recognition-using-tesseract-OCR

Visual Studio 2022

OpenCV 4.5.4

Tesseract-OCR 5.0.1

### Input image
![8](https://user-images.githubusercontent.com/94694035/158774664-7725e7dd-1241-4c9d-8ce4-47431aecc815.JPG)

### Image Processing
#### Gray
![gray](https://user-images.githubusercontent.com/94694035/162560518-81e4c020-36cd-49cc-96d0-1e4f2fec8077.PNG)
#### GaussianBlur
![gaussianBlur](https://user-images.githubusercontent.com/94694035/162560567-3d52d3a4-886f-4d93-af48-219b1a758e5c.PNG)
#### Canny
![canny](https://user-images.githubusercontent.com/94694035/162560577-927d12b2-eeff-4666-a380-314e3776d6d0.PNG)
#### Contours&Rectangles
![contours rectangles](https://user-images.githubusercontent.com/94694035/162560595-c06aa7be-f23a-4ba3-b630-14055bcdc253.PNG)
#### Rectangles on plate
![rectangles on plate](https://user-images.githubusercontent.com/94694035/162560599-7d063e7b-f5d3-4258-8a6a-5edacad8d16d.PNG)
#### Rotated
![Rotated](https://user-images.githubusercontent.com/94694035/162560611-28037222-aabe-4985-b78e-86f1fa676ada.PNG)

### Output image
![8-1](https://user-images.githubusercontent.com/94694035/158774767-50f30856-839c-4bf7-aad8-ead73518f394.jpg)


### Result
![car number print](https://user-images.githubusercontent.com/94694035/160058691-8a12241d-2989-4b7a-af63-4114a41d2df9.PNG)


Put the image you want to recognize in the carImage folder and change the factor of the imageProcessing function.

If the car license plate is not korean, change the regular expression in the printCarNumber function.
