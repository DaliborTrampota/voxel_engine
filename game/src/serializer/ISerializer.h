#pragma once


class ISerializer {
  public:
    virtual ~ISerializer() = default;

    virtual void serialize() = 0;
    virtual void deserialize() = 0;
};