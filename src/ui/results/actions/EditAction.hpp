// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#pragma once
#include "AbstractResultAction.hpp"
namespace javatb {
class EditAction final : public AbstractResultAction {
    Q_OBJECT
public:
    explicit EditAction(ResultRenderer* r, QObject* parent = nullptr);
};
} // namespace javatb
