#pragma once

#include "Angle.h"
#include "DataTypes.h"
#include "ViewRenderer.h"

class Player
{
public:
    Player(ViewRenderer *pViewRenderer, int iID);
    ~Player();

    void SetXPosition(int XPosition);
    void SetYPosition(int YPosition);
    void SetAngle(int Angle);
    void RotateLeft();
    void RotateRight();
    void RenderAutoMap();

    int GetID();
    int GetXPosition();
    int GetYPosition();

    Angle GetAngle();

    bool ClipVertexesInFOV(Vertex &V1, Vertex &V2, Angle &V1Angle, Angle &V2Angle);
    Angle AngleToVertex(Vertex &vertex);

protected:
    int m_iPlayerID;
    int m_XPosition;
    int m_YPosition;
    int m_FOV;
    int m_iRotationSpeed;
    int m_iMoveSpeed;

    Angle m_Angle;
    ViewRenderer *m_pViewRenderer;
};
