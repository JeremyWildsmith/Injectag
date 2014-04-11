#pragma once

namespace Injectag
{

class ChainedModule
{
public:
    ChainedModule();

    void link(ChainedModule* const next);
    void unlink();
    
    ChainedModule* next() const;
    bool hasNext() const;

protected:
    struct DriveStage
    {
        enum e
        {
            FeedQuery,
            Execution,
            ReadBack,
            End,
        };
    };

    DriveStage::e getDriveStage() const;

    virtual int getDrSize() const = 0;
    virtual int getIrSize() const = 0;

    virtual bool transmitIr(const bool dataIn) = 0;
    virtual bool transmitDr(const bool dataIn) = 0;

    virtual void linkedTo(const ChainedModule* neighbor) = 0;
    virtual void linkedFrom(const ChainedModule* neighbor) = 0;

    virtual void driveStateChanged(const DriveStage::e stage) = 0;

private:
    ChainedModule* m_next;
    DriveStage::e m_driveStage;

    void enterDriveStage(const DriveStage::e stage);

    friend class JtagDriver;
};

};