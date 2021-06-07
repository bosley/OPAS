# IMAGE

The image core is a set of objects that are used for the obtaining and processing of images.
Image Artifact

Class that handles the aggregating of points or labels to draw on-top of images
Image Frame

Wraps an OpenCV matrix with timestamp information
Image Frame ReceiverIf

Interface for objects that take in frames. This class is leveraged heavily by the objects laid out below.
Image ProcessIf

Interface to define objects that will act as an image processor. Can be given a Frame ReceiverIf so the resulting process can be viewed.
Producer

The primary image getting objects. Setup to read data from cameras/ images/ files/ etc. A producer is the means-by-which the application is fed image data
Stream

Taking in a producer, a stream is ticked by the main application to drive-along the processing of data from a producer. While the ticking can happen at any rate, a stream can limit the data input to a specified number of frames-per-second.
Image Processor

Is-a FrameReceiverIf, receives frames from a stream and executes any given ProcessIf on the image.
Image Process Result

The resulting data from a ProcessIf

