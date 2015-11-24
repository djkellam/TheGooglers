#!/bin/sh
if [ $DEPLOYMENT_LOCATION = "YES" ]
then
cp -R FPS_Multiplayer/Binaries/IOS/Payload/FPS_Multiplayer.app/ $DSTROOT/$LOCAL_APPS_DIR/FPS_Multiplayer.app
fi
