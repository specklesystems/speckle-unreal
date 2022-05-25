#pragma once

/**
 * 
 */
class SPECKLEUNREAL_API FBatchSender : public FRunnable
{

protected:

	FString ServerUrl;
	FString StreamId;
	FString AuthToken;
	int32 MaxBatchSizeMb;
	int32 MaxBufferLength;
	int32 ThreadCount;

	bool ShouldSendThreadRun = false;
	bool IsWriteComplete = false;
	FRunnableThread* SendingThread;
	FCriticalSection Lock_SendBuffer;
	TArray<TTuple<const FString, const FString>> SendBuffer;
	
public:

	FBatchSender(FString& ServerUrl, FString& StreamId, FString& AuthToken, int32 MaxBatchSizeMb=1, int32 MaxBufferLength = 10, int32 ThreadCount = 4);

	virtual ~FBatchSender() override;
	
	virtual uint32 Run() override;
	virtual void Stop() override;


	virtual void EnqueueSend(const FString& Id, const FString& Object);
	virtual void Flush();

protected:
	
	virtual void SendingThreadMain();
};
