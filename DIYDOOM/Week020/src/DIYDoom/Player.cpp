#include "Player.h"
#include <math.h>

Player::Player(ViewRenderer *pViewRenderer, int iID) : m_pViewRenderer(pViewRenderer), m_iPlayerID(iID), m_FOV(90), m_iRotationSpeed(4), m_iMoveSpeed(4), m_EyeLevel(41)
{
    m_ZPosition = m_EyeLevel;
    m_pWeapon = std::unique_ptr<Weapon>(new Weapon("PISGA0"));
}

Player::~Player()
{
}

void Player::Init(Thing thing)
{
    SetXPosition(thing.XPosition);
    SetYPosition(thing.YPosition);
    SetAngle(thing.Angle);
    m_HalfFOV = m_FOV / 2;
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

int Player::GetZPosition()
{
    return m_ZPosition;
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

void Player::SetZPosition(int ZPosition)
{
    m_ZPosition = ZPosition;
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

bool Player::ClipVertexesInFOV(Vertex &V1, Vertex &V2, Angle &V1Angle, Angle &V2Angle, Angle &V1AngleFromPlayer, Angle &V2AngleFromPlayer)
{
    V1Angle = AngleToVertex(V1);
    V2Angle = AngleToVertex(V2);

    Angle V1ToV2Span = V1Angle - V2Angle;

    if (V1ToV2Span >= 180)
    {
        return false;
    }

    // Rotate every thing.
    V1AngleFromPlayer = V1Angle - m_Angle;
    V2AngleFromPlayer = V2Angle - m_Angle;

    // Validate and Clip V1
    // shift angles to be between 0 and 90 (now virtualy we shifted FOV to be in that range)
    Angle V1Moved = V1AngleFromPlayer + m_HalfFOV;

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
        V1AngleFromPlayer = m_HalfFOV;
    }

    // Validate and Clip V2
    Angle V2Moved = m_HalfFOV - V2AngleFromPlayer;

    // Is V2 outside the FOV?
    if (V2Moved > m_FOV)
    {
        V2AngleFromPlayer = -m_HalfFOV;
    }

    V1AngleFromPlayer += 90;
    V2AngleFromPlayer += 90;

    return true;
}

void Player::MoveForward()
{
    m_XPosition += m_Angle.GetCosValue() * m_iMoveSpeed;
    m_YPosition += m_Angle.GetSinValue() * m_iMoveSpeed;
}

void Player::MoveLeftward()
{
    m_XPosition -= m_Angle.GetCosValue() * m_iMoveSpeed;
    m_YPosition -= m_Angle.GetSinValue() * m_iMoveSpeed;
}

void Player::RotateLeft()
{
    m_Angle += (0.1875f * m_iRotationSpeed);
}

void Player::RotateRight()
{
    m_Angle -= (0.1875f * m_iRotationSpeed);
}

void Player::Fly()
{
    m_ZPosition += 1;
}

void Player::Sink()
{
    m_ZPosition -= 1;
}

float Player::DistanceToPoint(Vertex &V)
{
    // We have two points, where the player is and the vertex passed.
    // To calculate the distance just use "The Distance Formula"
    // distance = square root ((X2 - X1)^2 + (y2 - y1)^2)
    return sqrt(pow(m_XPosition - V.XPosition, 2) + pow(m_YPosition - V.YPosition, 2));
}

int Player::GetFOV()
{
    return m_FOV;
}

void Player::Think(int iSubSectorHieght)
{
    m_ZPosition = iSubSectorHieght + m_EyeLevel;
}

void Player::Render(uint8_t *pScreenBuffer, int iBufferPitch)
{
    m_pWeapon->Render(pScreenBuffer, iBufferPitch);
}