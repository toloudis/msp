#include "oglView.h"
#if 0
/// This helper modifies an interval [a, b] such that the midpoint of the interval
/// is maintained, and the length of the interval is multiplied by 'scale'.
void centeredRescale(double& a, double& b, double scale)
{
    double middle = (a + b) * 0.5;
    double newA = middle + (a - b) * 0.5 * scale;
    double newB = middle + (b - a) * 0.5 * scale;

    a = newA;
    b = newB;
}

static const maVector3d CARDINAL_AXES[oglView::NUM_AXIS_TYPES] = {
                                             maVector3d(1.0,  0.0,  0.0),
                                             maVector3d(0.0,  1.0,  0.0),
                                             maVector3d(0.0,  0.0,  1.0),
                                             maVector3d(-1.0,  0.0,  0.0),
                                             maVector3d(0.0, -1.0,  0.0),
                                             maVector3d(0.0,  0.0, -1.0)};

static const float FRAME_PADDING = 1.2;

oglView::oglView()
: mProjectionType(PERSPECTIVE)
, mLeft(0)
, mRight(0)
, mTop(0)
, mBottom(0)
, mScreenLeft(-1)
, mScreenRight(1)
, mScreenTop(1)
, mScreenBottom(-1)
, mAspectRatio(1)
{
    reset();
}

oglView::~oglView()
{
}

oglView::oglView(oglView const &src)
: mPosition(src.mPosition)
, mReference(src.mReference)
, mUp(src.mUp)
, mProjectionType(src.mProjectionType)
, mNear(src.mNear)
, mFar(src.mFar)
, mFov(src.mFov)
, mLetterboxAspectRatio(src.mLetterboxAspectRatio)
, mHaveLetterboxAspectRatio(src.mHaveLetterboxAspectRatio)
, mLeft(src.mLeft)
, mRight(src.mRight)
, mTop(src.mTop)
, mBottom(src.mBottom)
, mScreenLeft(src.mScreenLeft)
, mScreenRight(src.mScreenRight)
, mScreenTop(src.mScreenTop)
, mScreenBottom(src.mScreenBottom)
, mOrthoXAxis(src.mOrthoXAxis)
, mOrthoYAxis(src.mOrthoYAxis)
, mOrthoZAxis(src.mOrthoZAxis)
, mAspectRatio(src.mAspectRatio)
, mAffineOffset(src.mAffineOffset)
, mAffineRotation(src.mAffineRotation)
, mAffineScale(src.mAffineScale)
{
}

oglView const &
oglView::operator=(oglView const &src)
{
    mPosition = src.mPosition;
    mReference = src.mReference;
    mUp = src.mUp;
    mFov = src.mFov;
    mLetterboxAspectRatio = src.mLetterboxAspectRatio;
    mHaveLetterboxAspectRatio = src.mHaveLetterboxAspectRatio;
    mNear = src.mNear;
    mFar = src.mFar;
    mLeft = src.mLeft;
    mRight = src.mRight;
    mBottom = src.mBottom;
    mTop = src.mTop;
    mScreenLeft = src.mScreenLeft;
    mScreenRight = src.mScreenRight;
    mScreenTop = src.mScreenTop;
    mScreenBottom = src.mScreenBottom;
    mOrthoXAxis = src.mOrthoXAxis;
    mOrthoYAxis = src.mOrthoYAxis;
    mOrthoZAxis = src.mOrthoZAxis;
    // note: don't copy the src context, continue to use our own
    mProjectionType = src.mProjectionType;
    mAspectRatio = src.mAspectRatio;
    mAffineOffset = src.mAffineOffset;
    mAffineRotation = src.mAffineRotation;
    mAffineScale = src.mAffineScale;

    return *this;
}

bool
oglView::operator ==(const oglView& other) const
{
    return mPosition == other.mPosition &&
        mReference == other.mReference &&
        mUp == other.mUp &&
        mFov == other.mFov &&
        mLetterboxAspectRatio == other.mLetterboxAspectRatio &&
        mHaveLetterboxAspectRatio == other.mHaveLetterboxAspectRatio &&
        mNear == other.mNear &&
        mFar == other.mFar &&
        mLeft == other.mLeft &&
        mRight == other.mRight &&
        mBottom == other.mBottom &&
        mTop == other.mTop &&
        mScreenLeft == other.mScreenLeft &&
        mScreenRight == other.mScreenRight &&
        mScreenTop == other.mScreenTop &&
        mScreenBottom == other.mScreenBottom &&
        mOrthoXAxis == other.mOrthoXAxis &&
        mOrthoYAxis == other.mOrthoYAxis &&
        mOrthoZAxis == other.mOrthoZAxis &&
        mProjectionType == other.mProjectionType &&
        mAspectRatio == other.mAspectRatio &&
        mAffineOffset == other.mAffineOffset &&
        mAffineRotation == other.mAffineRotation &&
        mAffineScale == other.mAffineScale;
}

void
oglView::reset()
{
    // the default setup frames a sphere of diameter 105
    mPosition  = maVector3d(0, 0, 195.933);
    mReference = maVector3d(0, 0, 0);
    mUp        = maVector3d(0, 1, 0);
    mFov       = degToRad(30.0);
    mLetterboxAspectRatio = 1.0;
    mHaveLetterboxAspectRatio = false;
    mNear      = 145.933;
    mFar       = 245.933;
    mProjectionType = PERSPECTIVE;
    mScreenLeft = -1;
    mScreenRight = 1;
    mScreenTop = 1;
    mScreenBottom = -1;
    mAspectRatio = 1;

    // ortho snapping support
    mOrthoXAxis = PX;
    mOrthoYAxis = PY;
    mOrthoZAxis = PZ;


    mAffineOffset = maVector2d(0, 0);
    mAffineRotation = 0.0;
    mAffineScale = 1.0;

    mHaveLetterboxAspectRatio = false;
    mLetterboxAspectRatio = 1;
}

void
oglView::setPosition(maVector3d const &pos)
{
    if (mReference.eq(pos)) {
        DBG_ERROR("View ref and pos cannot be coincident");
    }
    mPosition = pos;
    fixUpVector();
}

maVector3d const &
oglView::position() const
{
    return mPosition;
}

void
oglView::setRefPoint(maVector3d const &ref)
{
    if (mPosition.eq(ref)) DBG_ERROR("View ref and pos cannot be coincident");
    mReference = ref;
    fixUpVector();
}

maVector3d const &
oglView::refPoint() const
{
    return mReference;
}

void
oglView::setPositionAndRef(maVector3d const &pos, maVector3d const &ref)
{
    if (pos.eq(ref)) DBG_ERROR("View ref and pos cannot be coincident");
    mPosition = pos;
    mReference = ref;
    fixUpVector();
}

void
oglView::setPositionAndRefAndUp(const maVector3d& pos, const maVector3d& ref, const maVector3d& up)
{
    if (pos.eq(ref)) {
        DBG_ERROR("View ref and pos cannot be coincident");
    }
    if (!REL_EQ_TOL(0, (ref - pos).dot(up), SMALLVALUE)) {
        DBG_ERROR("View direction and up vector must be perpendicular");
    }
    mPosition = pos;
    mReference = ref;

    mUp = up.unit();
}

maVector3d const &
oglView::upVector() const
{
    return mUp;
}

void
oglView::setFieldOfView(double fov)
{
    assert(mProjectionType == PERSPECTIVE);

    // (FOV>PI) arguably doesn't make sense, but we'll let that be for now and
    // just check for unarguably nonsensical values.
    assert(fov > 0 && fov <= 2*M_PI);

    mFov = fov;
}

double
oglView::fieldOfView() const
{
    return mFov;
}

void
oglView::setOrthoProjection(double fov)
{
    const maVector3d ray = mReference - mPosition;
    // thinking of the ray length and the radius of a circle
    const double r = ray.length();
    // theta is the half the FOV in radians
    const double theta = fov/2.0;
    // Think of a right triangle whose angle is theta
    // and whose adjacent side, (AB), is r units long.
    // We want the height of the opposite side (BC)
    //
    //                    C
    //                    .
    //                 .  .
    //              .     .
    //           .        .
    //        .          _.
    //     .  theta     | .
    //   ..................
    // A         r         B
    //
    // tan(theta) = opposite/adjacent.
    // So, opposite = tan(theta)*adjacent
    // To reiterate:
    // A = camera position
    // B = reference position
    // C = unknown
    //
    // offset is the length of the opposite side.
    // Its called offset because it is conceptually the number of
    // units (left,right,bottom,top) we're widening the view
    // about the view direction ray
    const double offset = r * tan(theta);

    // Code below assumes that you have snapped to nearest
    // ortho axis. Free ortho is not supported at the moment
    unsigned int xIndex;
    double xScale;
    getOrthoAxisIndexAndScale(mOrthoXAxis, xIndex, xScale);
    double xDelta = xScale * offset * mAspectRatio;
    mLeft = - xDelta;
    mRight = + xDelta;

    unsigned int yIndex;
    double yScale;
    getOrthoAxisIndexAndScale(mOrthoYAxis, yIndex, yScale);
    double yDelta = offset;
    mTop = + yDelta;
    mBottom = - yDelta;

    mProjectionType = ORTHOGRAPHIC;
}

void
oglView::setOrthoProjection(double left, double right, double bottom, double top)
{
    mLeft = left;
    mRight = right;
    mBottom = bottom;
    mTop = top;
    mProjectionType = ORTHOGRAPHIC;
}

void
oglView::updateOrthoAxes()
{
    maVector3d v = mPosition - mReference;
    v.normalize();
    maVector3d temp = mUp;
    temp.normalize();
    maVector3d right = temp.cross(v);
    maVector3d up = v.cross(right);

    unsigned int xAxis = 0, yAxis = 0, zAxis = 0;
    double max_dot = 0.0;
    double dot = 0.0;
    for (unsigned int iAxis = 0; iAxis < NUM_AXIS_TYPES; ++iAxis) {
        dot = v.dot(CARDINAL_AXES[iAxis]);
        if (dot > max_dot) {
            zAxis = iAxis;
            max_dot = dot;
        }
    }
    mOrthoZAxis = static_cast<AxisType>(zAxis);

    max_dot = 0.0;
    dot = 0.0;
    for (unsigned int iAxis = 0; iAxis < NUM_AXIS_TYPES; ++iAxis) {
        dot = up.dot(CARDINAL_AXES[iAxis]);
        if (dot > max_dot
            && iAxis != zAxis
            && iAxis != (zAxis + 3) % NUM_AXIS_TYPES) {
            yAxis = iAxis;
            max_dot = dot;
        }
    }
    mOrthoYAxis = static_cast<AxisType>(yAxis);

    max_dot = 0.0;
    dot = 0.0;
    for (unsigned int iAxis = 0; iAxis < NUM_AXIS_TYPES; ++iAxis) {
        dot = right.dot(CARDINAL_AXES[iAxis]);
        if (dot > max_dot
            && iAxis != zAxis
            && iAxis != (zAxis + 3) % NUM_AXIS_TYPES
            && iAxis != yAxis
            && iAxis != (yAxis + 3) % NUM_AXIS_TYPES) {
            xAxis = iAxis;
            max_dot = dot;
        }
    }
    mOrthoXAxis = static_cast<AxisType>(xAxis);
}

void
oglView::snapToNearestAxis()
{
    updateOrthoAxes();

    maVector3d v =  mPosition - mReference;
    double len = v.length();

    v = CARDINAL_AXES[mOrthoZAxis] * len;
    mPosition = mReference + v;
    mUp = CARDINAL_AXES[mOrthoYAxis];
}

void
oglView::setPerspectiveProjection(double fov)
{
    mFov = fov;
    mProjectionType = PERSPECTIVE;
}

void
oglView::setPerspectiveProjection()
{
    mProjectionType = PERSPECTIVE;
}

/// Set to show a subwindow of the projection
void
oglView::setScreenWindow(double left, double right, double bottom, double top)
{
    mScreenTop = top;
    mScreenBottom = bottom;
    mScreenLeft = left;
    mScreenRight = right;
}

void
oglView::setAspectRatio(double aspect)
{
    mAspectRatio = aspect;
}

void
oglView::setNearClipDistance(double d)
{
    mNear = d;
}

void
oglView::setFarClipDistance(double d)
{
    mFar = d;
}

// maybe we should use spherical coordinates instead!
static void
rotate_around_axis(maVector3d &x, maVector3d const &axis, double angle)
{
    maMatrix4x4 R = gmath::rotation<maMatrix4x4>(axis, -angle);
    x = x * R;
}

void
oglView::orbit(double dTheta, double dPhi)
{
    // note: this whole function should just use spherical coordinates directly.

    // this fudge factor prevents the vector (mReference - mPosition)
    // from being along the line defined by the y axis, (0, 1, 0)*t .
    // This situation would occur if the camera were to be straight up or straight down.
    // Were this to happen, fixUpVector() would produce degenerate values.
    const double fudge = 0.001;     // mmmm, double fudge.

    // math in here assumes this
    assert(!mReference.eq(mPosition));

    double dAzim = degToRad(dTheta);
    double dIncl = degToRad(dPhi);

    maVector3d lookAt, right;
    getFrame(lookAt, right);

    maVector3d orbitAxis(0, 1, 0); // TODO make this not hard-coded

    // the current inclination
    double incl = acos(orbitAxis.dot(mUp));

    if (orbitAxis.dot(lookAt) > 0.0) { // if the camera is facing down
        if (incl + dIncl > M_PI/2.0)
            // the change in inclination takes us across the pole - clamp so
            // that we stop orbiting at the pole
            dIncl = M_PI/2.0 - incl - fudge;
    } else { // if the camera is facing up
        if (incl - dIncl > M_PI/2.0)
            // the change in inclination takes us across the pole - clamp so
            // that we stop orbiting at the pole
            dIncl = -(M_PI/2.0 - incl - fudge);
    }


    // reposition camera
    maVector3d relPos = mPosition - mReference;
    rotate_around_axis(relPos, right, -dIncl);  // rotate camera around right vector
    rotate_around_axis(relPos, orbitAxis, dAzim); // rotate camera around orbit axis
    mPosition = mReference + relPos;

    // orient the camera
    rotate_around_axis(mUp, right, -dIncl); // rotate up vector around right vector
    rotate_around_axis(mUp, orbitAxis, dAzim); // rotate the up vector around orbit axis
}

void
oglView::rotate(const maMatrix4x4& rotation)
{
    // Rotate our view direction and up vector by this rotation
    maVector3d forward = mReference - mPosition;
    mUp = mUp * rotation;
    forward = forward * rotation;
    mReference = mPosition + forward;
}

void
oglView::getFrame(maVector3d &forward, maVector3d &right)
{
    forward = mReference - mPosition;
    forward.normalize();

    right = forward.cross(mUp);
    right.normalize();
}

void
oglView::tumble(double x1, double y1, double x2, double y2, const maVector4d& viewport)
{
    // We want to construct a rotation that takes point (x2, y2) to (x1, y1)
    // and apply this to our view and up directions.

    // Start by figuring out where (x1, y1) and (x2, y2) correspond to in view space
    maVector3d from = unproject(x2, y2, 0, viewport) - mPosition;
    maVector3d to = unproject(x1, y1, 0, viewport) - mPosition;

    // Want our 'from' and 'to' positions projected onto a unit sphere
    from.normalize();
    to.normalize();

    // Now we can directly construct the rotation we want
    maMatrix4x4 rotation = gmath::rotation<maMatrix4x4>(from, to);

    // Apply it
    rotate(rotation);
}

void
oglView::pitch(double angle)
{
    maVector3d forward, right;
    getFrame(forward, right);
    rotate(gmath::rotation<maMatrix4x4>(right, angle));
}

void
oglView::yaw(double angle)
{
    rotate(gmath::rotation<maMatrix4x4>(mUp, -angle));
}

void
oglView::roll(double angle)
{
    maVector3d forward, right;
    getFrame(forward, right);
    rotate(gmath::rotation<maMatrix4x4>(forward, -angle));
}

void
oglView::dolly(double r, double u, double f)
{
    // math in here assumes this
    assert(!mReference.eq(mPosition));

    maVector3d lookAt, right;
    getFrame(lookAt, right);

    if (!pdi::isZero(r) || !pdi::isZero(u)) {
        mReference += -right*r + mUp*u;
    }

    // make the camera always at least 0.0001 units from the reference point
    double d = (mReference - mPosition).length();
    d = std::max(d + f, 0.0001);
    mPosition = mReference - (lookAt * d);
}

void
oglView::getOrthoAxisIndexAndScale(AxisType aAxis, unsigned int & aIndex, double & aScale)
{
    switch (aAxis)
    {
    case PX:
        aIndex = 0;
        aScale = 1.0;
        break;
    case PY:
        aIndex = 1;
        aScale = 1.0;
        break;
    case PZ:
        aIndex = 2;
        aScale = 1.0;
        break;
    case NX:
        aIndex = 0;
        aScale = -1.0;
        break;
    case NY:
        aIndex = 1;
        aScale = -1.0;
        break;
    case NZ:
        aIndex = 2;
        aScale = -1.0;
        break;
    default:
        assert(false && "Invalid axis type");
    }
}

// Code in this function assumes that you have snapped to nearest
// ortho axis. Free ortho is not supported at the moment
void
oglView::panOrtho(double fromX, double fromY, double toX, double toY, const maVector4d& viewport)
{
    maVector3d from = unproject(fromX, fromY, 0, viewport);
    maVector3d to = unproject(toX, toY, 0, viewport);
    maVector3d delta = to - from;

    unsigned int xIndex;
    double xScale;
    getOrthoAxisIndexAndScale(mOrthoXAxis, xIndex, xScale);
    double xDelta = delta[xIndex];
    mPosition[xIndex] -= xDelta;
    mReference[xIndex] -= xDelta;

    unsigned int yIndex;
    double yScale;
    getOrthoAxisIndexAndScale(mOrthoYAxis, yIndex, yScale);
    double yDelta = delta[yIndex];
    mPosition[yIndex] -= yDelta;
    mReference[yIndex] -= yDelta;
}

void
oglView::zoomOrtho(double amount)
{
    // We want a multiplier that tends to 0 as amount gets highly negative,
    // equals 1 when amount is 0, and is an increasing function of amount.
    // Power curve works perfectly ... and also has the nice property that multiple
    // small mouse movements combine to give the same result as one large mouse
    // movement of the same total distance.
    // The base is relatively arbitrary, particularly as the amount passed in has an
    // arbitrary scale in the first place.
    double multiplier = pow(2.0, amount);

    centeredRescale(mLeft, mRight, multiplier);
    centeredRescale(mBottom, mTop, multiplier);
}

void
oglView::fixUpVector()
{
    // math in here assumes this
    assert(!mReference.eq(mPosition));

    // Have a go at calculating the up vector based on the assumption that we're not actually
    // looking directly up or down!
    maVector3d lookAt = mReference - mPosition;
    lookAt.normalize();
    maVector3d right = lookAt.cross(maVector3d(0, 1, 0));
    right.normalize();
    mUp = right.cross(lookAt);

    // Patch up the case where we are looking directly up or down.  In this case we arbitrarily choose
    // up direction of (0, 0, -1).  I'm not sure what happens here with perspective views.  The
    // motivation is to get an orthographic top/bottom view that works.
    if (mUp.eq(maVector3d(0, 0, 0))) {
        mUp = maVector3d(0, 0, -1);
    } else {
        mUp.normalize();
    }
}

void
oglView::setupGL() const
{
    setupModelviewAndProjection();
}

void
oglView::setupModelviewAndProjection() const
{
    // set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projection().asPointer());

    // set up modelView matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(modelView().asPointer());
}

void
oglView::frame(bbox::BBox3d const &bbox)
{
    if (mProjectionType == PERSPECTIVE) {
        // TODO: We need to actually project the bounding box extents into screen space and compute
        // the clipping (or how much to dolly back) from that, rather than using the absolute radius
        // of the bounding box.  This will let us properly frame objects that may have large non-cube
        // bounding boxes that have their shorter sides more aligned with the camera plane

        // The following is not final, we need those two iterations to get a proper result.
        // There is still something wrong with the algorithm...
        // If this doesn't work at all, use the old version.
#if 1
        framePerspective(bbox);
        framePerspective(bbox);
        framePerspective(bbox);
#else
        frame(bbox.getCenter(), (bbox.getMin() - bbox.getMax()).length()/2.0);
#endif
    } else {
        frameOrtho(bbox);
    }
}

void
oglView::extendRect(const maVector3d& p, std::vector<maVector3d>& rect) const
{
    // x
    rect[0][0] = std::min(p[0], rect[0][0]);
    rect[1][0] = rect[0][0];
    rect[2][0] = std::max(p[0], rect[2][0]);
    rect[3][0] = rect[2][0];

    // y
    rect[0][1] = std::min(p[1], rect[0][1]);
    rect[3][1] = rect[0][1];
    rect[1][1] = std::max(p[1], rect[1][1]);
    rect[2][1] = rect[1][1];

    // z
    rect[0][2] = std::min(p[2], rect[0][2]);
    rect[1][2] = rect[0][2];
    rect[2][2] = std::max(p[2], rect[2][2]);
    rect[3][2] = rect[2][2];
}


std::vector<maVector3d>
oglView::bBoxToScreenRect(const bbox::BBox3d& bbox) const
{
    maMatrix4x4 toScreen(modelView() * projection());

    const maVector3d& bboxMin = bbox.getMin();
    const maVector3d& bboxMax = bbox.getMax();

    // Transform each corner and create a new bounding box.
    maVector3d p;

    p.init(bboxMin[0], bboxMin[1], bboxMin[2]);
    p = toScreen.transformH(p);
    // initialize rect with first screen point
    std::vector<maVector3d> rect(4, p); // [bottomLeftFront, topLeftFront, topRightBack, bottomRightBack]

    p.init(bboxMin[0], bboxMin[1], bboxMax[2]);
    p = toScreen.transformH(p);
    // now extend rect
    extendRect(p, rect);

    p.init(bboxMin[0], bboxMax[1], bboxMin[2]);
    p = toScreen.transformH(p);
    extendRect(p, rect);

    p.init(bboxMin[0], bboxMax[1], bboxMax[2]);
    p = toScreen.transformH(p);
    extendRect(p, rect);

    p.init(bboxMax[0], bboxMin[1], bboxMin[2]);
    p = toScreen.transformH(p);
    extendRect(p, rect);

    p.init(bboxMax[0], bboxMin[1], bboxMax[2]);
    p = toScreen.transformH(p);
    extendRect(p, rect);

    p.init(bboxMax[0], bboxMax[1], bboxMin[2]);
    p = toScreen.transformH(p);
    extendRect(p, rect);

    p.init(bboxMax[0], bboxMax[1], bboxMax[2]);
    p = toScreen.transformH(p);
    extendRect(p, rect);

    return rect;
}

void
oglView::framePerspective(const bbox::BBox3d& bbox)
{
    std::vector<maVector3d> rect = bBoxToScreenRect(bbox);

    double projWidth = fabs(rect[3][0] - rect[0][0]);
    double projHeight = fabs(rect[1][1] - rect[0][1]);

    // padding
    if ((fabs(2.0 - projWidth) < 0.01) || (fabs(2.0 - projHeight) < 0.01)) {
        return;
    }

    const maMatrix4x4 toScreen(modelView() * projection());
    const maMatrix4x4 toWorld(toScreen.inverse());

    const maVector3d screenCenter((rect[2][0] - rect[0][0])/ 2.0 + rect[0][0],
                                    (rect[1][1] - rect[0][1])/ 2.0 + rect[0][1],
                                    (rect[2][2] - rect[0][2])/ 2.0 + rect[0][2]);
    // back project screen center
    const maVector3d worldCenter = toWorld.transformH(screenCenter);

    // Pan the view
    mPosition = mPosition - mReference + worldCenter;
    mReference = worldCenter;
    fixUpVector();

    // again get the screen rect
    rect = bBoxToScreenRect(bbox);

    projWidth = fabs(rect[3][0] - rect[0][0]);
    const double widthScaleFactor = projWidth / 2.0 * FRAME_PADDING;
    projHeight = fabs(rect[1][1] - rect[0][1]);
    const double heightScaleFactor = projHeight / 2.0 * FRAME_PADDING;

    // I've cut the next formula down to this. This basically is the back projection
    // of the object's screen width into world space and the resulting dolly amount.
    // But I reduce a lot of variables.
    const double curD = (worldCenter - mPosition).length();
    // take the bigger scale factor to dolly out
    const double maxScaleFactor = std::max(widthScaleFactor, heightScaleFactor);
    const double newD = curD * maxScaleFactor;

    dolly(0, 0, newD - curD);

    const double radius = (bbox.getMax() - bbox.getMin()).length() / 2.0;
    mNear = std::max(newD - radius, 0.001);
    mFar = newD + (radius * 2);
    fixUpVector();
}


void
oglView::frame(maVector3d const &center, double radius)
{
    if (mProjectionType == PERSPECTIVE) {
        // math in here assumes this
        assert(!mReference.eq(mPosition));

        mPosition = mPosition - mReference + center;

        // point ourselves at the new center
        mReference = center;

        double newD = 0.0;
        double curD = (mPosition - center).length();
        double aspectRatio = 1.0;

        if (mHaveLetterboxAspectRatio) {
            aspectRatio = mLetterboxAspectRatio;
        } else {
            aspectRatio = mAspectRatio;
        }
        // If our aspect ratio is > 1, it means our viewport is wider than it is high, so use
        // the height for framing.  Otherwise, we use the width as normal
        if (aspectRatio > 1.0) {
            newD = radius / tan(mFov / (aspectRatio * 2));
        } else {
            newD = radius / tan(mFov / 2);
        }

        // dolly out until the scene fits in the FOV
        dolly(0, 0, newD - curD);

        // reset the clipping planes
        mNear = std::max(newD - radius, 0.001);
        mFar = newD + (radius * 2);
        fixUpVector();
    } else {
        // Orthographic projection.
        bbox::BBox3d box(center, center);
        box.pad(maVector3d(radius, radius, radius));

        frameOrtho(box);
    }
}

// Code in this function assumes that you have snapped to nearest
// ortho axis. Free ortho is not supported at the moment
void
oglView::frameOrtho(bbox::BBox3d box)
{
    // this method currently assumes that viewing direction is an axis; doesn't
    // necessarily cope with freeform ortho views.

    // Position at the centre of one of the bounding box faces, such that we're looking
    // along the viewing direction and encompassing the whole scene.
    maVector3d dir, rightDir;
    getFrame(dir, rightDir);

    if (!box.hasVolume()) {
        const maVector3d size = box.getSize();
        maVector3d padding;
        double length = size.length() * .5;
        if (REL_EQ_TOL(0, size[0], SMALLVALUE)) {
            padding[0] = length;
        }
        if (REL_EQ_TOL(0, size[1], SMALLVALUE)) {
            padding[1] = length;
        }
        if (REL_EQ_TOL(0, size[2], SMALLVALUE)) {
            padding[2] = length;
        }
        box.pad(padding);
    }
    const maVector3d min = box.getMin();
    const maVector3d max = box.getMax();
    const maVector3d center = box.getCenter();

    // I want to put mReference at the center of the bounds,
    // move the mPosition to a point just outside the bounds.

    // First: get which axis is along the view direction.
    // This code assumes that camera direction "dir" is
    // the same as CARDINAL_AXES[mOrthoZAxis].
    double zScale;
    unsigned int zIndex;
    getOrthoAxisIndexAndScale(mOrthoZAxis, zIndex, zScale);

    static const double nearDist = 0.001;
    double farDist = box.getSize()[zIndex] + nearDist;
    mReference = center;
    mPosition = mReference - (dir * (farDist*0.5));

    // Now set up the ortho bounds:

    double xScale;
    unsigned int xIndex;
    getOrthoAxisIndexAndScale(mOrthoXAxis, xIndex, xScale);

    double yScale;
    unsigned int yIndex;
    getOrthoAxisIndexAndScale(mOrthoYAxis, yIndex, yScale);

    double maxSize = 0.5 * std::max(box.getSize()[xIndex], box.getSize()[yIndex]);

    double xDelta = xScale * maxSize * mAspectRatio;
    mLeft = - xDelta;
    mRight = + xDelta;

    double yDelta = maxSize;
    mTop = + yDelta;
    mBottom = - yDelta;

    // reset the clipping planes
    mNear = nearDist;
    mFar = farDist;
}

void
oglView::center(bbox::BBox3d const &bbox)
{
    if (mProjectionType == PERSPECTIVE) {
        maVector3d const & center = bbox.getCenter();
        double radius = (bbox.getMax() - bbox.getMin()).length() / 2.0;

        frame(center, radius);
    } else {
        frameOrtho(bbox);
    }
}

void
oglView::resetAffineTransformations()
{
    mAffineOffset = maVector2d(0,0);
    mAffineRotation = 0;
    mAffineScale = 1;
}

void
oglView::copyAffineTransformFrom(disp::View & aView)
{
    mAffineOffset = aView.getAffineOffset();
    mAffineRotation = aView.getAffineRotation();
    mAffineScale = aView.getAffineScale();
}

void
oglView::affineRotate(double aAffineRotation, maVector2d aAboutPoint, const maVector4d& viewport)
{
    // Handle overall view rotation
    mAffineRotation += aAffineRotation;

    // Compute about point
    maVector2d about = aAboutPoint - maVector2d(viewport[2], viewport[3])*0.5;
    about /= maVector2d(viewport[2], viewport[3]);

    // move offset to about point
    mAffineOffset -= about;

    // Rotate
    double s = sin(aAffineRotation);
    double c = cos(aAffineRotation);
    maVector2d r(mAffineOffset[0]*c - mAffineOffset[1]*s, mAffineOffset[0]*s + mAffineOffset[1]*c);

    // move offset back
    mAffineOffset = r;
    mAffineOffset += about;
}

void
oglView::affineScale(double aAffineScale, maVector2d aAboutPoint, const maVector4d& viewport)
{
    // Handle overall view scale
    mAffineScale *= aAffineScale;

    // Compute about point
    maVector2d about = aAboutPoint - maVector2d(viewport[2], viewport[3])*0.5;
    about /= maVector2d(viewport[2], viewport[3]);

    // move offset to about point
    mAffineOffset -= about;

    // Scale
    maVector2d r = mAffineOffset * aAffineScale;

    // move offset back
    mAffineOffset = r;
    mAffineOffset += about;
}

maVector3d
oglView::unproject(double x, double y, double z, const maVector4d& viewport) const
{
    maVector3d clip_coord = pixelToClip(x,y,2*z-1, viewport);
    return clipToWorld( clip_coord );
}

float
oglView::depthGL(unsigned int x, unsigned int y, const maVector4d& viewport)
{
    y = static_cast<unsigned int>(viewport[3] - y - 1);

    GLfloat z;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

    return z;
}

maVector3d
oglView::project(maVector3d const &p, const maVector4d& viewport) const
{
    const maMatrix4x4 w2c = modelView() * projection();
    const maVector3d c = w2c.transformH(p);

    return maVector3d(viewport[0] + viewport[2]*(c[0]+1)/2.0,
                        viewport[1] + viewport[3]*(c[1]+1)/2.0,
                        (c[2]+1)/2.0);
}

maVector3d
oglView::pixelToClip(double x,
                  double y,
                  double z,
                  const maVector4d& viewport) const
{
    // remove this assert when we are certain this behaves well with nonzero viewport offsets.
    assert(viewport[0] == 0);
    assert(viewport[1] == 0);
    double clip_x = (2*(x-viewport[0]))/
                    viewport[2] - 1.0;
    // gl's pixel coordinates are "upside-down" w.r.t most windowing
    // systems. The origin is on the lower-left of the buffer.  Flip it.
    double clip_y = 1.0 -
        (2*(y+1+viewport[1]))/viewport[3];

    return maVector3d(clip_x,clip_y,z);
}

maVector3d
oglView::clipToWorld(const maVector3d& clipPoint) const
{
    maMatrix4x4 toScreen(modelView() * projection());
    maMatrix4x4 toWorld(toScreen.inverse());
    return toWorld.transformH(clipPoint);
}

maMatrix4x4
oglView::affineTransform() const
{
    maMatrix4x4 offset;
    double offsetY = -mAffineOffset[1];   // Flip y coordinate
    offset.setToTranslation(maVector3d(2.0*mAffineOffset[0], 2.0*offsetY, 0.0));

    maMatrix4x4 invAspectScale, aspectScale, rot;
    invAspectScale.setToScale(maVector3d(1.0/mAspectRatio, 1.0, 1.0));
    aspectScale.setToScale(maVector3d(mAspectRatio, 1.0, 1.0));
    rot.setToRotation(gmath::Z, -mAffineRotation);  // Rotate around negative Z axis, so clockwise is positive

    maMatrix4x4 scale;
    scale.setToScale(maVector3d(mAffineScale, mAffineScale, 1.0));

    return scale * aspectScale * rot * invAspectScale * offset;
}

maMatrix4x4
oglView::projection() const
{
    // viewport has to be valid for this to mean anything
    assert(mAspectRatio > 0);

    maMatrix4x4 retval;

    switch (mProjectionType) {

    case PERSPECTIVE:
    {
        double aspectRatioMultiplier = 1;
        if (mHaveLetterboxAspectRatio) {
            if (mLetterboxAspectRatio < mAspectRatio) {
                aspectRatioMultiplier = mAspectRatio / mLetterboxAspectRatio;
            }
        }

        const double dx    = tan(mFov * 0.5);
        const double dw    = dx * mNear * aspectRatioMultiplier;
        const double dh    = dw / mAspectRatio;

        const double right = dw * mScreenRight;
        const double top   = dh * mScreenTop;

        const double bottom = dh * mScreenBottom;
        const double left   = dw * mScreenLeft;

        const double A = (2*mNear)/(right-left);
        const double B = (2*mNear)/(top-bottom);
        const double C = -(mFar+mNear)/(mFar-mNear);
        const double D = -(2*mFar*mNear)/(mFar-mNear);

        const double E = (right+left)/(right-left);
        const double F = (top+bottom)/(top-bottom);

        // ROW major (opposite of GL)
        retval = maMatrix4x4( A   ,  0.0 ,  0.0 ,  0.0 ,
                               0.0 ,  B   ,  0.0 ,  0.0 ,
                               E   ,  F   ,  C   , -1.0 ,
                               0.0 ,  0.0 ,  D   ,  0.0 );
        break;
    }

    case ORTHOGRAPHIC:
    {
        double right = mRight;
        double left = mLeft;
        double top = mTop;
        double bottom = mBottom;

        double orthoAspect = (right - left) / (top - bottom);

        if (mAspectRatio > orthoAspect) {
            // Viewport is wide ... need to stretch right/left to keep image in proportion
            centeredRescale(left, right, mAspectRatio / orthoAspect);
        } else if (orthoAspect > mAspectRatio) {
            // Viewport is narrow ... need to stretch top/bottom to keep image in proportion
            centeredRescale(bottom, top, orthoAspect / mAspectRatio);
        }

        const double A = 2.0/(right-left);
        const double B = 2.0/(top-bottom);
        const double C = -2.0/(mFar-mNear);
        const double Tz = -(mFar+mNear)/(mFar-mNear);
        const double D = -(right+left)/(right-left);
        const double E = -(top+bottom)/(top-bottom);

        retval = maMatrix4x4( A   , 0.0 , 0.0 , 0.0 ,
                               0.0 , B   , 0.0 , 0.0 ,
                               0.0 , 0.0 , C   , 0.0 ,
                               D ,   E   , Tz  , 1.0 );

        break;
    }

    default:
        assert(mProjectionType == PERSPECTIVE ||
               mProjectionType == ORTHOGRAPHIC);
        break;
    }

    // Apply the affine transform directly to the projection matrix
    retval = retval * affineTransform();

    return retval;
}

/// get the left, right, top and bottom of the near plane
void
oglView::getNearPlane(double& left, double& right, double& top, double& bottom)
{
    switch (mProjectionType) {

    case PERSPECTIVE:
    {
        // we assume a regular frustum centered on the viewing axis (none of this
        // skewed perspective nonsense)
        double aspectRatioMultiplier = 1;
        if (mHaveLetterboxAspectRatio) {
            if (mLetterboxAspectRatio < mAspectRatio) {
                aspectRatioMultiplier = mAspectRatio / mLetterboxAspectRatio;
            }
        }

        const double dx    = tan(mFov * 0.5);

        const double dw    = dx * mNear * aspectRatioMultiplier;
        const double dh    = dw / mAspectRatio;

        right = dw * mScreenRight;
        top   = dh * mScreenTop;

        bottom = dh * mScreenBottom;
        left   = dw * mScreenLeft;
        break;
    }
    case ORTHOGRAPHIC:
    {
        right = mRight;
        left = mLeft;
        top = mTop;
        bottom = mBottom;

        double orthoAspect = (right - left) / (top - bottom);

        if (mAspectRatio > orthoAspect) {
            // Viewport is wide ... need to stretch right/left to keep image in proportion
            centeredRescale(left, right, mAspectRatio / orthoAspect);
        } else if (orthoAspect > mAspectRatio) {
            // Viewport is narrow ... need to stretch top/bottom to keep image in proportion
            centeredRescale(bottom, top, orthoAspect / mAspectRatio);
        }
    }
    default:
        assert(mProjectionType == PERSPECTIVE ||
               mProjectionType == ORTHOGRAPHIC);
        break;
    }
}

maMatrix4x4
oglView::modelView() const
{
    // math in here assumes this
    assert(!mReference.eq(mPosition));

    maMatrix4x4 retval;

    const maVector3d f = (mReference - mPosition).unit();
    const maVector3d s = (f.cross(mUp)).unit();
    const maVector3d u = s.cross(f);

    // ROW major (opposite of GL)
    retval = maMatrix4x4( s[0] ,  u[0] , -f[0] ,  0.0 ,
                           s[1] ,  u[1] , -f[1] ,  0.0 ,
                           s[2] ,  u[2] , -f[2] ,  0.0 ,
                           0.0  ,   0.0 ,   0.0 ,  1.0 );
    retval.accumTranslation(-mPosition);

    return retval;
}

gmath::Frustum<double>
oglView::frustum() const
{
    assert((mProjectionType != ORTHOGRAPHIC) && "Orthographic oglView::frustum not yet supported.");

    maVector3d position(mPosition);
    maVector3d direction(mReference);

    direction -= position;
    direction.normalize();

    maVector3d up(mUp);
    maVector3d left = up.cross(direction);

    up.normalize();
    left.normalize();

    // we assume a regular frustum centered on the viewing axis (none of this
    // skewed perspective nonsense)
    double aspectRatioMultiplier = 1;
    if (mHaveLetterboxAspectRatio) {
        if (mLetterboxAspectRatio < mAspectRatio) {
            aspectRatioMultiplier = mAspectRatio / mLetterboxAspectRatio;
        }
    }
    const double dx    = tan(mFov * 0.5);

    // TODO: account for mScreenLeft and mScreenTop?
    left *= dx * mNear * aspectRatioMultiplier;
    up   *= dx * mNear * aspectRatioMultiplier / mAspectRatio;

    return gmath::Frustum<double>(
        position,
        direction,
        up, left,
        mNear, mFar,
        (mProjectionType == ORTHOGRAPHIC));
}

View
oglView::viewLinearInterpolator(const disp::View& start, const disp::View& end, double progress)
{
     disp::View toRet(end);

     // As a linear combination of start and end values
     double sw = 1.0 - progress; // start weight
     double ew = progress;       // end weight

     if (sw < 1e-3) {
         return toRet;
     }

     maVector3d forwardStart = (start.refPoint() - start.position());
     maVector3d forwardEnd = (end.refPoint() - end.position());
     double forwardLengthStart = forwardStart.length();
     double forwardLengthEnd = forwardEnd.length();
     forwardStart.normalize();
     forwardEnd.normalize();

     maVector3d upStart = start.upVector().unit();
     maVector3d upEnd = end.upVector().unit();

     maMatrix3x3 sourceBasis(forwardStart, upStart, forwardStart.cross(upStart));
     maMatrix3x3 targetBasis(forwardEnd, upEnd, forwardEnd.cross(upEnd));
     maRotation slerped = gmath::slerp(maRotation(sourceBasis), maRotation(targetBasis), progress);
     maMatrix3x3 interp(slerped);

     maVector3d pos = sw * start.position() + ew * end.position();
     maVector3d ref = pos + interp.row(0) * (sw * forwardLengthStart + ew * forwardLengthEnd);
     maVector3d up = interp.row(1);

     double near = MIN(start.nearClipDistance(), end.nearClipDistance());
     double far = MAX(start.farClipDistance(), end.farClipDistance());

     double fov = sw * start.fieldOfView() + ew * end.fieldOfView();

     try {
         toRet.setPositionAndRefAndUp(pos, ref, up);
     } catch (...) {}    // if we fail, we'll just use the end value
     toRet.setNearClipDistance(near);
     toRet.setFarClipDistance(far);
     if (toRet.projectionType() == disp::oglView::PERSPECTIVE
         && fov > 0.0 && fov <= 2*M_PI ) {
         /// @todo Smooth transition from perspective to ortho
         toRet.setFieldOfView(fov);
     }

     return toRet;
}


void
oglView::debugPrint() const
{
    std::cerr << "Position   = " << mPosition << std::endl;
    std::cerr << "Reference  = " << mReference << std::endl;
    std::cerr << "Up         = " << mUp << std::endl;
    std::cerr << "Aspect     = " << mAspectRatio << std::endl;
    std::cerr << "Near       = " << mNear << std::endl;
    std::cerr << "Far        = " << mFar << std::endl;
    std::cerr << "Projection = ";

    switch (mProjectionType) {
    case PERSPECTIVE:
        std::cerr << "Perspective" << std::endl;
        std::cerr << "FOV (deg)  = " << radToDeg(mFov) << std::endl;
        break;

    case ORTHOGRAPHIC:
        std::cerr << "Orthographic" << std::endl;
        std::cerr << "Left       = " << mLeft << std::endl;
        std::cerr << "Right      = " << mRight << std::endl;
        std::cerr << "Bottom     = " << mBottom << std::endl;
        std::cerr << "Top        = " << mTop << std::endl;
        break;
    }
}
#endif
