#ifndef CAMERA
#define CAMERA

#include <math/Matrix4.h>
#include <math/Vector3.h>

class Camera
{
	public:
		Camera();
		~Camera();

		// Rotations
		void Yaw(double parAngle);
		void Pitch(double parAngle);
		void Roll(double parAngle);
		// Translation
		void Translate(const Vector3& parDir);

		const Matrix4& GetViewMatrix() const{ return FViewMatrix;}
		const Matrix4& GetProjectionMatrix() const{ return FProjection;}
		const Matrix4& GetProjectionViewMatrix() const{ return FProjection;}

		void DefinePerspective(float parFovy, float parAspect, float parNear, float parFar);
		void ChangeNoticed();
		bool HasChanged() {return FHasChanged;}

		
	protected:
		Matrix4 FViewMatrix;
		Matrix4 FProjection;
		Matrix4 FProjectionView;
		bool FHasChanged;
};
#endif //CAMERA