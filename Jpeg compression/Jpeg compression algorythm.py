import numpy as np
from PIL import Image
from scipy.fftpack import dct, idct
import json

def RGB_2_YCbCr(imArray):
    retArray = np.empty_like(imArray, np.float)
    retArray[:,:,0] = 0 + (0.299*imArray[:,:,0])+(0.587*imArray[:,:,1])+(0.114*imArray[:,:,2])
    retArray[:,:,1] = 128-(0.1687*imArray[:,:,0])-(0.3312*imArray[:,:,1])+(0.5*imArray[:,:,2])
    retArray[:,:,2] = 128 + (0.5*imArray[:,:,0])-(0.4186*imArray[:,:,1])-(0.0813*imArray[:,:,2])
    return np.uint8(retArray)

def YcbCr_2_RGB(imArray):
    retArray = np.empty_like(imArray, np.float)
    imArray = imArray.astype(np.float)
    retArray[:,:,0] = imArray[:,:,0]+1.402*(imArray[:,:,2]-128)
    retArray[:,:,1] = imArray[:,:,0]-0.3441*(imArray[:,:,1]-128)-0.7141*(imArray[:,:,2]-128)
    retArray[:,:,2] = imArray[:,:,0]+1.772*(imArray[:,:,1]-128)
    np.putmask(retArray, retArray > 255, 255)
    np.putmask(retArray, retArray < 0, 0)
    return np.uint8(retArray)

def Down_Sample(imgLayer,rowSampleFactor, colSampleFactor):
    retLayer = imgLayer[0::rowSampleFactor, 0::colSampleFactor]
    return retLayer

def Up_Sample(imgLayer, rowSampleFactor, colSampleFactor):
    retArray = np.repeat(np.repeat(imgLayer, colSampleFactor, axis=1), rowSampleFactor, axis=0)
    return retArray

def Layer_DCT(imgLayer, rowSize=8, colSize=8):
    retLayer = np.empty_like(imgLayer, np.float)
    imgLayer = imgLayer.astype(np.float) - 128 
    for i in np.arange(0,imgLayer.shape[0],rowSize):
        for j in np.arange(0,imgLayer.shape[1],colSize):
            #rows first, then the columns
            retLayer[i:(i+rowSize), j:(j+colSize)] = dct(dct(imgLayer[i:(i+rowSize), j:(j+colSize)], axis = 0, norm='ortho'), axis=1, norm='ortho')
    return retLayer

def Layer_IDCT(imgLayer, rowSize=8, colSize=8):
    retLayer = np.empty_like(imgLayer, np.float)
    for i in np.arange(0,imgLayer.shape[0],rowSize):
        for j in np.arange(0,imgLayer.shape[1],colSize):
            retLayer[i:(i+rowSize), j:(j+colSize)] = idct(idct(imgLayer[i:(i+rowSize), j:(j+colSize)], axis = 0, norm='ortho'), axis=1, norm='ortho')
    retLayer += 128
    #after backwards quantization some pixels can become out of borders of color restriction (a pixel can come out to be more than 255)
    #it is especially noticeable on the border of white color with others
    np.putmask(retLayer, retLayer > 255, 255)
    np.putmask(retLayer, retLayer < 0, 0)
    return retLayer

def Layer_Quantization(imgLayer, quantMatrix, rowSize = 8, colSize = 8):
    retLayer = np.empty_like(imgLayer)
    for i in np.arange(0,imgLayer.shape[0],rowSize):
        for j in np.arange(0,imgLayer.shape[1],colSize):
            sBlock = retLayer[i:(i+rowSize), j:(j+colSize)].shape
            retLayer[i:(i+rowSize), j:(j+colSize)] = np.around(np.divide(imgLayer[i:(i+rowSize), j:(j+colSize)],quantMatrix[0:sBlock[0],0:sBlock[1]]))
            retLayer[i:(i+rowSize), j:(j+colSize)] = np.multiply(retLayer[i:(i+rowSize),j:(j+colSize)],quantMatrix[0:sBlock[0], 0:sBlock[1]])
    return retLayer
    #insert image folder
ImgFolder = ''
    #insert image name (should be uncompressed .png file format
ImgName = u''
    #read the compressionMatrix from json
jsonFile = open(ImgFolder+u'compresstionMatrix.json')
jsonData = json.load(jsonFile)
yMatrix = np.array(jsonData['yMatrix'])
cbMatrix = np.array(jsonData['cbMatrix'])
crMatrix = np.array(jsonData['crMatrix'])
yDsArgs = jsonData['yDsArgs']
cbDsArgs = jsonData['cbDsArgs']
crDsArgs = jsonData['crDsArgs']


MyImg = Image.open(ImgFolder+ImgName)
ImgData = np.asarray(MyImg)
#Color scheme from RGB to YCbCr
ImgData = RGB_2_YCbCr(ImgData)

#Image layer downsampling
yLayer=Down_Sample(ImgData[:,:,0],yDsArgs[0],yDsArgs[1])
cbLayer=Down_Sample(ImgData[:,:,1],cbDsArgs[0],cbDsArgs[1])
crLayer=Down_Sample(ImgData[:,:,2],crDsArgs[0],crDsArgs[1])

#DCT for each layer
yLayer = Layer_DCT(yLayer,yMatrix.shape[0],yMatrix.shape[1])
cbLayer = Layer_DCT(cbLayer,cbMatrix.shape[0],cbMatrix.shape[1])
crLayer = Layer_DCT(crLayer,crMatrix.shape[0],crMatrix.shape[1])

#Quantization
yLayer = Layer_Quantization(yLayer, yMatrix)
cbLayer = Layer_Quantization(cbLayer, cbMatrix)
crLayer = Layer_Quantization(crLayer, crMatrix)

#IDCT for each layer
yLayer = Layer_IDCT(yLayer,yMatrix.shape[0],yMatrix.shape[1])
cbLayer = Layer_IDCT(cbLayer,cbMatrix.shape[0],cbMatrix.shape[1])
crLayer = Layer_IDCT(crLayer,crMatrix.shape[0],crMatrix.shape[1])

#Upsampling the pixels
ImgData[:,:,0] = Up_Sample(yLayer,yDsArgs[0],yDsArgs[1])
ImgData[:,:,1] = Up_Sample(cbLayer,cbDsArgs[0],cbDsArgs[1])
ImgData[:,:,2] = Up_Sample(crLayer,crDsArgs[0],crDsArgs[1])

#Form image from channels
Image.fromarray(ImgData[:,:,0]).show()
Image.fromarray(ImgData[:,:,1]).show()
Image.fromarray(ImgData[:,:,2]).show()

#Saving the image after applying jpeg transformations on it
ImgData = YcbCr_2_RGB(ImgData)
Image.fromarray(ImgData).save(ImgFolder+u'jpegd.png')
