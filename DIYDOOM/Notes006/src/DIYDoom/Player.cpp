#include "Player.h"

Player::Player(int iID) : m_iPlayerID(iID)
{
}

Player::~Player()
{
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

int Player::GetAngle()
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
    m_Angle = Angle;
}
