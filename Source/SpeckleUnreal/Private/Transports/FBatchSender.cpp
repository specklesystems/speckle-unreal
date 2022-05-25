#include "FBatchSender.h"

FBatchSender::FBatchSender(FString& ServerUrl, FString& StreamId, FString& AuthToken, int32 MaxBatchSizeMb,
	int32 MaxBufferLength, int32 ThreadCount)
	: ServerUrl(ServerUrl)
	, StreamId(StreamId)
	, AuthToken(AuthToken)
	, MaxBatchSizeMb(MaxBatchSizeMb)
	, MaxBufferLength(MaxBufferLength)
	, ThreadCount(ThreadCount)
{
	SendingThread = FRunnableThread::Create(this, TEXT("Speckle Transport Send Thread"), 0);
	ensure(SendingThread != nullptr);
}

FBatchSender::~FBatchSender()
{
	if (SendingThread)
	{
		SendingThread->Kill();
		delete SendingThread;
		SendingThread = nullptr;
	}
}


uint32 FBatchSender::Run()
{
	while(true)
	{
		if(!ShouldSendThreadRun) return 0;

		TArray<TTuple<FString, FString>> Buffer;
		FScopeLock Lock(&Lock_SendBuffer);
		if(SendBuffer.Num() > 0)
		{
			Buffer.Append(SendBuffer);
			SendBuffer.Empty();
		}
		else
		{
			IsWriteComplete = true;
		}

		if(Buffer.Num() <= 0)
		{
			FPlatformProcess::Sleep(0.1);
			continue;
		}

		TArray<FString> ObjectIds;
		ObjectIds.Reserve(Buffer.Num());
		for(const auto& Item: Buffer)
		{
			ObjectIds.Add(Item.Key);
		}

		//TODO only send objects that aren't already on the server.
		//TMap<FString, bool> HasObjects = API.HasObjects(StreamId, ObjectIds);

		//TODO api upload
		//API.UploadObjects(StreamId, Buffer);


		
	}
}

void FBatchSender::Stop()
{
	ShouldSendThreadRun = false;
}

void FBatchSender::EnqueueSend(const FString& Id, const FString& Object)
{
	FScopeLock Lock(&Lock_SendBuffer);
	SendBuffer.Emplace(Id, Object);
	IsWriteComplete = false;
}
