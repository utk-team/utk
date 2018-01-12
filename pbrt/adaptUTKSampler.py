import re
import sys
import math

#arguments
#filename.pbrt pbrtSampler nspp cropwindowCoordinates

#input .pbrt file
filename = sys.argv[1]
imgres = sys.argv[2]
bounces = sys.argv[3]
samplefile = sys.argv[4]
globality = sys.argv[5]

imagestring = str('#define IMGRES ' + imgres)
bouncestring = str('#define NBBOUNCES ' + bounces)
samplestring = str('#define FILENAME ' + '"' + samplefile + '"')
globalitystring = str('readPtsFromFile' + globality + '(pts, nSamples, proj2D, IMGRES, IMGRES, p.x, p.y);')

# Read in the file
filedata = None
with open(filename, 'r+') as file :
    filedata = file.read()

# Replace the image size
filedata = re.sub(r'#define IMGRES.+',imagestring, filedata)

#Replace the integrator bounces
filedata = re.sub(r'#define NBBOUNCES.+',bouncestring, filedata)

#Replace the samples filename
filedata = re.sub(r'#define FILENAME.+',samplestring, filedata)

#Replace the globality function
filedata = re.sub(r'readPtsFromFileLocal\(pts, nSamples, proj2D, IMGRES, IMGRES, p.x, p.y\);',globalitystring, filedata)
filedata = re.sub(r'readPtsFromFileLocalCP\(pts, nSamples, proj2D, IMGRES, IMGRES, p.x, p.y\);',globalitystring, filedata)
filedata = re.sub(r'readPtsFromFileGlobal\(pts, nSamples, proj2D, IMGRES, IMGRES, p.x, p.y\);',globalitystring, filedata)

# Write the file out again
with open(filename, 'w') as file:
    file.write(filedata)
    
    
#all the arguments containing crop window coordinates
#args = map(float, sys.argv[4:])
#store crop window coordinates in simple variables
#x1 = sys.argv[4]
#x2 = sys.argv[5]
#y1 = sys.argv[6]
#y2 = sys.argv[7]
#cropwindowstring = str('"float cropwindow" [' + x1 + ' ' + x2 + ' ' + y1 + ' ' + y2 + ']')

#stringFileNameLocation = re.search("string filename", filedata)
#WorldBeginLocation = re.search("WorldBegin", filedata).start()
#IntegratorMLTLocation = re.search("mlt", filedata)
#SamplerLocation = re.search("Sampler", filedata)



#if IntegratorMLTLocation is not None:
    #if SamplerLocation is None and sampler == 'stratified':
        #nsamples = nsamples*nsamples
    #filedata = re.sub('Integrator "mlt" .+','Integrator "mlt" "integer mutationsperpixel" ' + str(nsamples), filedata)

# Replace the cropwindow with the input values in the .pbrt file
#filedata = re.sub(r'"float cropwindow".+',cropwindowstring, filedata)


