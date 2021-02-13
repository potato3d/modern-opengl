#pragma once
#include <GL/glut.h>

class FPS
{
public:
	FPS()
	{
		_updateIntervalSec = 0.5f;
		_numFrames = 0;
		_prevMS = 0;
	}

	bool update(float& fps)
	{
		int currMS = glutGet(GLUT_ELAPSED_TIME);
		float elapsedSec = (currMS - _prevMS) * 1e-3f;
		++_numFrames;

		if(elapsedSec < _updateIntervalSec)
		{
			return false;
		}
		else
		{
			fps = (float)_numFrames / elapsedSec;
			_numFrames = 0;
			_prevMS = currMS;
			return true;
		}
	}

private:
	float _updateIntervalSec;
	unsigned int _numFrames;
	int _prevMS;
};
