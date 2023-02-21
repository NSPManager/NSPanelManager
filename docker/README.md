# Default settings
Default admin page login:  
Username: root  
Password: password123

# Scripts

## docker-build_and_run_dev.sh
Will build a container and run it. It will not run the default application but instead drop the user in a shell inside the container. After the user exits the container, the container will be removed.

## docker-build_and_run.sh
Will build a container and run it in detached mode.

## docker-console.sh
This can be used to obtain a console in a running container started by docker-build_and_run.sh

## docker-stop_and_rm.sh
This will stop any container running and remove it.