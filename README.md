#DTIReg

##What is it?

DTI-Reg is an open-source C++ application that performs pair-wise DTI registration either by: 
 - Method 1: Using scalar FA map to drive the registration.
 - Method 2: Directly registering the tensor images with DTI-TK (currently not implemented)

Method 1:
 - Scalar map FA computation via dtiprocess
 - Optional: Linear registration via BRAINSFit (None, Rigid, Affine -default-)
 - Optional: Warping via BRAINSFit/BRAINSDemonWarp (BSpline, Diffeomorphic, LogDemons -default-)
    - Histogram matching is enabled
    - The linear registration is used to initialize the deformation field.
 - DTI resampling via ResampleDTI

Method 2:  (currently not implemented)
 - Registration/warping via DTI-TK

##License

See License.txt

##More information

https://www.nitrc.org/projects/dtireg/
