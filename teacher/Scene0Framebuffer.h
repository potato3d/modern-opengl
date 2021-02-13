#pragma once
#include <AABB.h>
#include <ShaderData.h>

class Scene
{
public:
    bool initialize()
    {
        return true;
    }

    const AABB& getBounds()
    {
        return _bounds;
    }

	void draw(const CameraData& /*cameraData*/)
    {
		// empty
    }

private:
    AABB _bounds;
};
