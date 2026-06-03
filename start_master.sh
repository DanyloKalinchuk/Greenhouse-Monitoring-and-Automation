#!/bin/bash

git pull || exit 1
cmake --build ./build --parallel || exit 1

./build/scada_process &
SCADA_PID=$!

echo "SCADA process ID: $SCADA_PID"
trap "kill $SCADA_PID" EXIT

source venv/bin/activate
python src/master/web/greenhouse_web_app/manage.py runserver 0.0.0.0:8000