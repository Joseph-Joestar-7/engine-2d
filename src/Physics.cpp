#include "Physics.h"

sf::Vector2f Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto& aPos = a->getComponent<CTransform>().pos;
    auto& bPos = b->getComponent<CTransform>().pos;

    auto& aBox = a->getComponent<CBoundingBox>();
    auto& bBox = b->getComponent<CBoundingBox>();

    float dx = std::abs(aPos.x - bPos.x);
    float dy = std::abs(aPos.y - bPos.y);

    float ox = (aBox.halfSize.x + bBox.halfSize.x) - dx;
    float oy = (aBox.halfSize.y + bBox.halfSize.y) - dy;

    if (ox > 0 && oy > 0)
        return { ox, oy };

    return { 0.f, 0.f };
}

sf::Vector2f Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto& aPrev = a->getComponent<CTransform>().prevPos;
    auto& bPrev = b->getComponent<CTransform>().prevPos;

    auto& aBox = a->getComponent<CBoundingBox>();
    auto& bBox = b->getComponent<CBoundingBox>();

    float dx = std::abs(aPrev.x - bPrev.x);
    float dy = std::abs(aPrev.y - bPrev.y);

    float ox = (aBox.halfSize.x + bBox.halfSize.x) - dx;
    float oy = (aBox.halfSize.y + bBox.halfSize.y) - dy;

    if (ox > 0 && oy > 0)
        return { ox, oy };

    return { 0.f, 0.f };
}
