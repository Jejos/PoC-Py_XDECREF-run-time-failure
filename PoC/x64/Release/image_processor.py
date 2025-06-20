import cv2
import MyModule

def process_image():
    img = cv2.imread('./img')  # enabled --> crash
    img = None                 # overwrite img --> it still crashes
                               # disable imread --> no crash
    MyModule.Diagnosis(img)
