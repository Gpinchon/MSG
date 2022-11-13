#include "GameEntity.hpp"

class CrispyWall : public GameEntity {
public:
    CrispyWall(Level &);
    static std::shared_ptr<CrispyWall> Create(Level &);
    virtual void Die() override;
    virtual void Update(float step) override {};

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return std::shared_ptr<CrispyWall>(new CrispyWall(*this));
    }
};