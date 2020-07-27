#include "GameObject.h"

GameObject::GameObject() : position(0.f, 0.f), size(1.f, 1.f), velocity(0.f), color(1.f), rotation(0.f), sprite(), isSolid(false), destroyed(false)
{
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
	: position(pos), size(size), velocity(velocity), color(color), rotation(0.f), sprite(sprite), isSolid(false), destroyed(false)
{
}

void GameObject::draw(SpriteRenderer& renderer)
{
	renderer.drawSprite(this->sprite, this->position, this->size, this->rotation, this->color);
}
