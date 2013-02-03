#pragma once

#include "Core/ma/maAxisBox.hpp"
#include "Core/ma/maMatrix4x4.hpp"
#include "Core/ma/maVector4d.hpp"
#include "Core/ma/maVector3d.hpp"
#include "Core/ma/maVector2d.hpp"

/// This view class sets the modelView and projection matrices in GL based
/// on a simple camera model.
class oglView
{
public:
	/// The projection types supported by View()
	enum ProjectionType { PERSPECTIVE, ORTHOGRAPHIC };
	enum AxisType { PX = 0, PY, PZ, NX, NY, NZ, NUM_AXIS_TYPES };

	/// Creates an unbound view.  Such a View may be used with clients that
	/// manage gl state externally, by binding a valid gl context and then
	/// calling View::setupGL() to set up the modelView and perspective
	/// matrices.
	oglView();

	/// Destructor.
	virtual ~oglView();

	/// Copy constructor.
	oglView(const oglView & src);

	/// Copies the view parameters.
	/// @note This copies ONLY the view parameters.  In particular, it does
	/// not reparent the view to the source view's Context, and since the
	/// viewport parameters are tied to the context, those aren't copied
	/// either.
	const oglView &operator=(const oglView &src);

	// Equality testing
	bool operator ==(const oglView& other) const;
	bool operator !=(const oglView& other) const { return !operator ==(other); }

	/// Sets the position of the viewpoint.
	/// @throw except::ValueError If called with a position that is
	/// coincident with the View's current reference point.
	/// @sa View::setPositionAndRef
	void setPosition(maVector3d const &pos);
	/// Returns the position of the viewpoint.
	maVector3d const &position() const;

	/// The view orbits around the reference point.
	/// @throw except::ValueError If called with a position that is
	/// coincident with the View's current position.
	/// @sa View::setPositionAndRef
	void setRefPoint(maVector3d const &pos);
	/// Returns the reference point for the view.
	maVector3d const &refPoint() const;

	/// Sets both the position and reference point of the camera
	/// simultaneously.  If you are setting both the position and reference
	/// points, it's better to use this instead of calling View::setPosition
	/// and View::setRefPoint separately.  Doing the latter will necessitate
	/// redundent checking of camera parameters, and also impose the
	/// somewhat artificial constraint that the position and reference point
	/// never be coincident (even in the intermediate state between when
	/// setPosition and setReference are called).
	/// NOTE this will nuke your up vector, which is almost never what you want!
	/// Typically better to call setPositionAndRefAndUp
	void setPositionAndRef(maVector3d const &pos, maVector3d const &ref);

	/// Sets position and reference point, with given up vector.  If you have a complete
	/// view transform to specify, use this rather than just setPositionAndRef [which will
	/// assume a (0,1,0) up-vector].  You must make sure the up vector is perpendicular to
	/// the viewing direction.
	void setPositionAndRefAndUp(const maVector3d& pos,
								const maVector3d& ref,
								const maVector3d& up);

	/// Returns the up vector for the view.
	/// This value is calculated and cannot be set manually.
	maVector3d const &upVector() const;

	/// Sets the field of view (in radians).  The default FOV is 30 degrees.  This is used as
	/// a horizontal field of view - i.e. the angle subtended by the left and right edges of
	/// viewport.  Note that gluPerspective works with a y fov instead.
	virtual void setFieldOfView(double fov);
	/// Returns the field of view (in radians)
	double fieldOfView() const;

	/// Convert a y (top-bottom) field of view to an x (left-right) field of view, for a
	/// viewport of given dimensions.
	static double fieldOfViewYToX(double fovY, double width, double height) {
		return 2 * atan(tan(fovY / 2) * width / height);
	}

	/// Set this if you're using letterboxing on your viewport to frame a specific aspect
	/// ratio, no matter how the viewport is resized.  This will cause the View to compute
	/// its projection matrix such that the field of view corresponds to the letterboxed
	/// area, not the entire viewport.
	double letterboxAspectRatio() const { return mHaveLetterboxAspectRatio ? mLetterboxAspectRatio : 1; }
	void setLetterboxAspectRatio(double ratio) { mLetterboxAspectRatio = ratio; mHaveLetterboxAspectRatio = true; }
	void resetLetterboxAspectRatio() { mHaveLetterboxAspectRatio = false; }

	/// Set the aspect ratio (typically the width/height of the viewport)
	void setAspectRatio(double aspect);
	/// @return the aspect ratio of the whole view.
	double aspectRatio() { return mAspectRatio; }

	/// Set the distance from the camera of the near clip plane
	void setNearClipDistance(double d);
	/// Return the distance from the camera of the near clip plane
	double nearClipDistance() const { return mNear; }
	/// Set the distance from the camera of the far clip plane
	void setFarClipDistance(double d);
	/// Return the distance from the camera of the far clip plane
	double farClipDistance() const { return mFar; }

	/// get the left, right, top and bottom of the near plane
	void getNearPlane(double& left, double& right, double& top, double& bottom);

	/// Orbits the camera around the reference point.
	/// @param dTheta The change in azimuth (in degrees).
	/// @param dPhi The change in inclination (in degrees).
	virtual void orbit(double dTheta, double dPhi);


	/// Retrieve the camera's frame of reference (returns unit vectors)
	/// Note, mUp is the final axis, already stored.
	void getFrame(maVector3d &forward, maVector3d &right);

	/// Tumbles the camera (i.e. maintains its position and just rotates it around)
	/// The parameters are in screen space, the idea being that the user can drag to
	/// tumble such that an object under the mouse stays under the mouse.
	/// @param x1 x-coordinate of tumble start point (screen-space)
	/// @param y1 y-coordinate of tumble start point (screen-space)
	/// @param x2 x-coordinate of tumble end point (screen-space)
	/// @param y2 y-coordinate of tumble end point (screen-space)
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	virtual void tumble(double x1, double y1, double x2, double y2, const maVector4d& viewport);

	/// Pitch camera (look up/down)
	/// @param angle Positive means up
	virtual void pitch(double angle);

	/// Yaw camera (look left/right)
	/// @param angle Positive means right
	virtual void yaw(double angle);

	/// Roll camera
	/// @param angle Positive means clockwise
	virtual void roll(double angle);

	/// Dollies the camera.  Dollying is done in camera space units, so "1"
	/// will move the camera 1 unit right/up/back in camera space.
	/// @param right Moves the camera along the right vector.  Positive
	/// units move the camera to the right.
	/// @param up Moves the camera along the up vector.  Positive units move
	/// the camera upwards.
	/// @param back Moves the camera along the viewing direction.  Positive
	/// units move the camera backwards.
	virtual void dolly(double right, double up, double back);

	/// Moves the camera in its view plane in an orthographic view.
	/// The inputs are screen-space positions, and we move the view such that
	/// the same part of the image stays under the mouse as the user drags.
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	void panOrtho(double fromX, double fromY, double toX, double toY, const maVector4d& viewport);

	/// Changes the width of the orthographic projection.  Positive amounts "zoom in",
	/// i.e. decrease the width so that objects appear larger.  Negative amounts do
	/// the opposite.  Some playing around may be required to get the scale of amount
	/// to feel right.
	void zoomOrtho(double amount);

	/// Makes the gl calls that actually set up the modelView and
	/// perspective matrices.
	void setupGL() const;

	/// setup the modelview and projection matrices
	void setupModelviewAndProjection() const;

	/// return the projection used by this view
	oglView::ProjectionType projectionType() const { return mProjectionType; }

	/// Set projection type to orthographic, with given field of view.
	/// LJH added this because previously, the orthographic projection was
	/// calculated from mFov every time, but I need it to be defined with
	/// left,right,bottom,top parameters as you'd expect.  This method allows
	/// the old code to continue setting it from a FOV, but maybe it can be
	/// eliminated altogether.
	void setOrthoProjection(double fov);

	/// Set this view to an orthographic projection.  The left/right/bottom/top
	/// numbers are used as *minimum* extents.  The actual extents may be larger
	/// as needed to keep the image 'square' (i.e. not stretched).  That depends
	/// on the aspect ratio of the viewport.
	void setOrthoProjection(double left, double right, double bottom, double top);

	/// returns left, right, bottom, top
	maVector4d getOrthoExtents() const {
		return maVector4d((float)mLeft, (float)mRight, (float)mBottom, (float)mTop);
	}

	/// returns which axis is pointing towards global X, Y, Z
	AxisType getOrthoXAxis() const { return mOrthoXAxis; }
	AxisType getOrthoYAxis() const { return mOrthoYAxis; }
	AxisType getOrthoZAxis() const { return mOrthoZAxis; }

	/// Snaps this view to the nearest major axis passing through
	/// the reference point, also adjusts the up-vector.
	/// This should normally be called when you switch from perspective
	/// mode to orthographic mode.
	void snapToNearestAxis();

	/// Set this view into perspective projection, specifying the field of view
	void setPerspectiveProjection(double fov);

	/// Set this view into perspective projection, using the same field of view as the last time
	/// it was perspective.
	void setPerspectiveProjection();

	/// Set to show a subwindow of the projection.  The full window spans the range
	/// left=-1, right=1, bottom=-1, top=1 and those are the defaults.  Set to a
	/// subrectangle to show a portion of the camera's full frame.
	void setScreenWindow(double left, double right, double bottom, double top);

	/// Resets the view to some known default state.  The default setup has
	/// the camera at about (0, 0, 200) in world space, framing a sphere of
	/// diameter 105.
	void reset();

	/// Back-project a position from raster space into world space.
	/// @param x The x coordinate of the point (in pixels).
	/// @param y The y coordinate of the point (in pixels).
	/// @param z The z coordinate of the point in screen space.  The default
	/// for this is 0 (i.e. the view plane)
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	/// @return The world-space point on the view plane that would project
	/// onto (x,y,z) in raster space.
	/// @note Do not invert y before passing it in.
	maVector3d unproject(double x, double y, double z, const maVector4d& viewport) const;

	/// Find the depth of the pixel at position x,y
	/// @param x The x coordinate of the point (in pixels).
	/// @param y The y coordinate of the point (in pixels).
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	/// @return z coordinate of the point in screen space as determined from the depth buffer.
	/// @note Do not invert y before passing it in.
	float depthGL(unsigned int x, unsigned int y, const maVector4d& viewport);

	/// Project a position from world space into raster space.
	/// @param p Position in world space.
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	/// @return The world-space point on the view plane that would project
	/// onto (x,y) in raster space.
	/// @warning Calling this function will result in a call to setupGL.
	/// This needs to be changed at some point, but for now it should be
	/// noted that calling this can potentially change the viewpoint.
	maVector3d project(maVector3d const &p, const maVector4d& viewport) const;

	/// Project a coordinate from pixel coordinates to clip coordinates
	/// Input is from pixel before the y has been flipped
	/// Output flips y.
	/// It also shifts x and y by viewport and scales them between -1 and +1.
	/// @param pixel_x The x coordinate of the point (in pixels).
	/// @param pixel_y The y coordinate of the point (in pixels).
	/// @param z The z coordinate is simply passed through unchanged.
	///   It should range between -1 and 1
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	/// @return The clip point on the view plane that would project
	/// onto (x,y,z) in pixel space.
	maVector3d pixelToClip(double pixel_x,
							 double pixel_y,
							 double z,
							 const maVector4d& viewport) const;

	/// Projects a point from clip space into world space
	maVector3d clipToWorld(const maVector3d& clipPoint) const;

	/// Sets the camera to frame the given bbox.
	/// @throw except::ValueError If the bbox center is coincident with the
	/// View's current position.
	void frame(maAxisBox const &bbox);

	/// Implementation of bbox framing for perspective view
	void framePerspective(const maAxisBox& bbox);

	/// Sets the camera to frame an area of the given radius, centered on
	/// the given point.
	/// @throw except::ValueError If the given center point is coincident
	/// with the View's current position.
	void frame(maVector3d const &center, double radius);

	maMatrix4x4 affineTransform() const;
	maMatrix4x4 projection() const;
	maMatrix4x4 modelView() const;

	/// Get a gmath::Frustum for this View
	//gmath::Frustum<double> frustum() const;

	void center(maAxisBox const &bbox);

	/// Resets affine transformations to identity
	void resetAffineTransformations();

	/// Copy affine xform from another view to this one
	void copyAffineTransformFrom(oglView & aDisp);

	/// @param aOffset The amount to offset, in viewport pixels
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	void affineOffset(const maVector2d &aOffset, const maVector4d& viewport) { mAffineOffset += maVector2d(aOffset[0]/viewport[2], aOffset[1]/viewport[3]); }
	maVector2d getAffineOffset() { return mAffineOffset; }

	/// Rotate (in radians, clockwise) about the center of the affine transform
	void affineRotate(double aAffineRotation) { mAffineRotation += aAffineRotation; }
	double getAffineRotation() { return mAffineRotation; }

	/// Rotate (in radians, clockwise) about a given point, in view coordinates
	/// @param aAffineRotation The amount to rotate, in radians
	/// @param aAboutPoint The center point of the rotation, in viewport pixels
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	void affineRotate(double aAffineRotation, maVector2d aAboutPoint, const maVector4d& viewport);

	/// Scale about the center of the affine transform
	void affineScale(double aAffineScale) { mAffineScale *= aAffineScale; }
	double getAffineScale() { return mAffineScale; }

	/// Scale about a given point, in view coordinates
	/// @param aAffineScale The scaling factor
	/// @param aAboutPoint The center point of the scaling, in viewport pixels
	/// @param viewport The viewport in pixels (xoffset, yoffset, xwidth, yheight)
	void affineScale(double aAffineScale, maVector2d aAboutPoint, const maVector4d& viewport);

	/// Interpolate from aStart to aEnd, given a progress between 0.0 and 1.0
	/// @param start The beginning view
	/// @param end The end, or target view to animate to
	/// @param progress How far long (between 0.0 and 1.0) the interpolation is
	static oglView viewLinearInterpolator(const oglView& start, const oglView& end, double progress);

	void debugPrint() const;
private:

	/// Fixes the up vector whenever the camera is moved.
	/// Call this after changing mPosition or mReference.
	void fixUpVector();

	/// Rotate the camera's frame of reference (not changing position)
	/// Caller is responsible for ensuring the matrix actually is a rotation!
	void rotate(const maMatrix4x4& rotation);

	/// Implementation of bbox framing for orthographic view
	void frameOrtho(maAxisBox box);

	/// Update ortho axes - updates which worldspace major axes
	/// line up most closely with the screenspace major axes.
	/// This is taken from lib dv and how current gen ortho views work.
	void updateOrthoAxes();

	/// This is a helper for mouse interactions - tells you which way are
	/// the major axes pointing so you can update the ortho extents accordingly
	void getOrthoAxisIndexAndScale(AxisType aAxis, unsigned int & aIndex, double & aScale);

	/// extend rect by point in screen space
	void extendRect(const maVector3d& p, std::vector<maVector3d>& rect) const;
	/// get 4 min/max screen points of bounding box
	std::vector<maVector3d> bBoxToScreenRect(const maAxisBox& bbox) const;

	// -----------------
	// View parameters
	// -----------------

	/// The position of the viewpoint.
	maVector3d mPosition;
	/// The view will orbit around the look-at point.
	maVector3d mReference;
	/// The viewpoint up vector.
	maVector3d mUp;


	// ----------------------
	// Projection parameters
	// ----------------------
	ProjectionType mProjectionType;

	// Near, far plane distances - used in both projection modes
	double mNear;
	double mFar;

	/// Field of view (radians) - only used in perspective mode
	double mFov;

	double mLetterboxAspectRatio;
	bool mHaveLetterboxAspectRatio;

	// Left, right, top, bottom - only used in orthographic mode
	double mLeft;
	double mRight;
	double mTop;
	double mBottom;

	// Left, right, top, bottom in screen coordinates
	// "full" view spans (-1,-1)..(1,1)
	double mScreenLeft;
	double mScreenRight;
	double mScreenTop;
	double mScreenBottom;

	// The following values keep track of which worldspace major axes
	// line up most closely with the screenspace major axes.
	// Each value can range from 0 to 5, as follows:
	// 0 = +X, 1 = +Y, 2 = +Z, 3 = -X, 4 = -Y, 5 = -Z
	// This is adapted from lib dv and how current gen ortho views work.
	AxisType mOrthoXAxis;
	AxisType mOrthoYAxis;
	AxisType mOrthoZAxis;

	// ----------------------
	// Viewport parameters
	// ----------------------

	double mAspectRatio;

	// ------------------------------
	// 2D affine transform parameters
	// ------------------------------

	/// The amount to offset (in normalized viewport coordinates) the view
	maVector2d mAffineOffset;

	/// Rotation (radians) about the center of the view
	double mAffineRotation;

	/// Scale about the center of the view
	double mAffineScale;
};
