#include "Player.h"
#include <math.h>

Player::Player(ViewRenderer *pViewRenderer, int iID) : m_pViewRenderer(pViewRenderer), m_iPlayerID(iID), m_FOV(90), m_iRotationSpeed(4), m_iMoveSpeed(2)
{
}

Player::~Player()
{
}

void Player::Init(Thing thing)
{
    SetXPosition(thing.XPosition);
    SetYPosition(thing.YPosition);
    SetAngle(thing.Angle);
}

int Player::GetID()
{
    return m_iPlayerID;
}

int Player::GetXPosition()
{
    return m_XPosition;
}

int Player::GetYPosition()
{
    return m_YPosition;
}

Angle Player::GetAngle()
{
    return m_Angle;
}

void Player::SetXPosition(int XPosition)
{
    m_XPosition = XPosition;
}

void Player::SetYPosition(int YPosition)
{
    m_YPosition = YPosition;
}

void Player::SetAngle(int Angle)
{
    m_Angle = (float)Angle;
}

Angle Player::AngleToVertex(Vertex &vertex)
{
    float Vdx = vertex.XPosition - m_XPosition;
    float Vdy = vertex.YPosition - m_YPosition;

    Angle VertexAngle(atan2f(Vdy, Vdx) * 180.0f / PI);

    return VertexAngle;
}

bool Player::ClipVertexesInFOV(Vertex &V1, Vertex &V2, Angle &V1Angle, Angle &V2Angle)
{
    V1Angle = AngleToVertex(V1);
    V2Angle = AngleToVertex(V2);

    Angle V1ToV2Span = V1Angle - V2Angle;

    if (V1ToV2Span >= 180)
    {
        return false;
    }

    // Rotate every thing.
    V1Angle = V1Angle - m_Angle;
    V2Angle = V2Angle - m_Angle;

    Angle HalfFOV = m_FOV / 2;

    // Validate and Clip V1
    // shift angles to be between 0 and 90 (now virtualy we shifted FOV to be in that range)
    Angle V1Moved = V1Angle + HalfFOV;

    if (V1Moved > m_FOV)
    {
        // now we know that V1, is outside the left side of the FOV
        // But we need to check is Also V2 is outside.
        // Lets find out what is the size of the angle outside the FOV
        Angle V1MovedAngle = V1Moved - m_FOV;

        // Are both V1 and V2 outside?
        if (V1MovedAngle >= V1ToV2Span)
        {
            return false;
        }

        // At this point V2 or part of the line should be in the FOV.
        // We need to clip the V1
        V1Angle = HalfFOV;
    }

    // Validate and Clip V2
    Angle V2Moved = HalfFOV - V2Angle;

    // Is V2 outside the FOV?
    if (V2Moved > m_FOV)
    {
        V2Angle = -HalfFOV;
    }

    V1Angle += 90;
    V2Angle += 90;

    return true;
}

void Player::RotateLeft()
{
    m_Angle += (0.1875f * m_iRotationSpeed);
}

void Player::RotateRight()
{
    m_Angle -= (0.1875f * m_iRotationSpeed);
}

void Player::RenderAutoMap()
{
    m_pViewRenderer->SetDrawColor(255, 0, 0);

    m_pViewRenderer->DrawLine(m_XPosition, m_YPosition, m_XPosition + 5, m_YPosition + 5);
}