# Cloud Computing, Fall 2025 Semester

This repository contains the source code, scripts, and documentation for a Cloud Computing learning project, developed in Ubuntu Linux using command-line tools and cloud platforms. The project is structured as a series of weekly labs to build foundational skills in cloud concepts and deployment. Git and GitHub usage begins from Week 03 for version control and repository management.

## Project Overview
- **Path**: `/home/deepblue/cloud_computing`
- **Environment**: Ubuntu Linux (e.g., VirtualBox VM), Python (Flask), Heroku CLI, OpenSSH, Vagrant
- **Purpose**: Learn cloud computing fundamentals through hands-on labs, with outputs verified via terminal logs and cloud dashboards.
- **Structure**: Weekly scripts and command histories called in lab sessions to demonstrate concepts.

## Week 01: Course Introduction and Cloud Computing Overview
### Objectives
- Understand the syllabus, tools, and basic terminology.
- Gain an overview of cloud computing history, benefits, and key players (e.g., AWS, Azure, Google Cloud).

### Content
- Reviewed course outline: Focus on service models, deployment strategies, and hands-on deployments.
- Explored core concepts: Scalability, elasticity, virtualization, and economic models.


## Week 02: Cloud Service Models
### Objectives
- Differentiate between service models: IaaS (Infrastructure as a Service), CaaS (Container as a Service), PaaS (Platform as a Service), FaaS (Function as a Service), and SaaS (Software as a Service).
- Understand responsibilities, use cases, and trade-offs for each model.

### Content
1. **`week02_service_models.md`** (or notes in Markdown):
   - IaaS: Full control over OS and apps (e.g., AWS EC2); user manages runtime, middleware, and data.
   - CaaS: Container orchestration (e.g., Kubernetes on Google Cloud); abstracts infrastructure but manages containers.
   - PaaS: Managed platform for app deployment (e.g., Heroku); handles runtime and middleware.
   - FaaS: Serverless execution (e.g., AWS Lambda); pay-per-use, no server management.
   - SaaS: Fully managed apps (e.g., Google Workspace); end-user consumption only.
   - Example: Diagram or table summarizing models (e.g., responsibility matrix).
2. **Hands-on Lab: PaaS Deployment with Heroku and Flask**:
   - Created a simple Flask app (`app.py`):
     ```python
     from flask import Flask
     app = Flask(__name__)

     @app.route('/')
     def hello():
         return "Hello from PaaS on Heroku!"

     if __name__ == '__main__':
         app.run()
     ```
   - Created a `requirments.txt` file and a `Procfile` file.
   - Prepared for deployment (full Git-based push covered in Week 03): `heroku create flask-hello-paas`.


## Week 03: Cloud Deployment Models
### Objectives
- Explore deployment strategies: Public, Private, Hybrid, and Community clouds.
- Understand security, compliance, and scalability implications.
- Introduce Git and GitHub for version control, including repository setup and basic commands.

### Content
1. **`week03_deployment_models.md`** (summary notes):
   - Public: Shared resources (e.g., AWS public cloud); cost-effective but less control.
   - Private: Dedicated infrastructure (e.g., on-premises VMware); high security for sensitive data.
   - Hybrid: Mix of public/private (e.g., burst to public during peaks); best for flexibility.
   - Community: Shared by specific groups (e.g., government clouds); balances cost and compliance.
   - Example: Use case scenarios, like a bank using hybrid for core vs. analytics workloads.
2. **Hands-on Lab: IaaS Part 1 - OpenSSH Setup**:
   - Installed OpenSSH server: `sudo apt update && sudo apt install openssh-server -y`.
   - Configured: Started service (`sudo systemctl start ssh`).
   - Tested: From local terminal, `ssh deepblue@localhost` (or remote VM IP).
   - Captured history: `history >> command.txt`.
3. **Git and GitHub Introduction**:
   - Installed Git: `sudo apt install git -y`.
   - Configured identity: `git config --global user.name "Your Name"` and `git config --global user.email "your.email@example.com"`.
   - Initialized repository: `git init`, added files (`git add .`), committed (`git commit -m "Initial commit"`), and pushed to GitHub (`git remote add origin <repo-url> && git push -u origin main`).

## Week 04: Data Centers and Cloud Architecture
### Objectives
- Understand physical and logical data center infrastructure components.
- Learn about server hardware, networking, cooling, and power management.
- Explore virtualization technologies and cloud architecture patterns.
- Study scalability, fault tolerance, and geographic distribution of cloud services.

### Content
1. **Theory: Data Centers and Cloud Architecture**:
   - Data center components: Servers, storage, networking equipment, cooling systems, power distribution.
   - Virtualization concepts: Hypervisors, virtual machines, resource allocation and management.
   - Cloud architecture patterns: Multi-tier architectures, microservices, load balancing, auto-scaling.
   - Geographic distribution: Availability zones, regions, content delivery networks (CDN).
   - Fault tolerance: Redundancy, backup systems, disaster recovery planning.

2. **Hands-on Lab: IaaS with Vagrant - Web Server Deployment**:
   - **Environment Setup**: Used Vagrant with VirtualBox to create reproducible IaaS environment.
   - **Configuration Files**:
     - `Vagrantfile`: Configured Ubuntu 24.04 VM with 2GB RAM, 2 CPUs, port forwarding (80→8080).
     - `bootstrap.sh`: Shell provisioning script for automated Apache installation and configuration.
     - `index.html`: Simple web content to verify successful deployment.
   
   - **Key Features Implemented**:
     - **VM Configuration**: 
       ```ruby
       config.vm.box = "bento/ubuntu-24.04"
       config.vm.provider "virtualbox" do |vb|
         vb.name = "IaaS-WebServer"
         vb.memory = "2048"
         vb.cpus = 2
       end
       ```
     - **Network Setup**: Port forwarding and private network configuration.
     - **Automated Provisioning**: Apache web server installation and configuration.
     - **Shared Folders**: Synchronization between host and guest systems.
   
   - **Deployment Process**:
     1. `vagrant up`: Launch and provision VM automatically.
     2. Access web server via `http://localhost:8080` or `http://192.168.33.10`.
     3. Verify deployment with custom HTML content.
     4. `vagrant destroy`: Clean up resources when done.
   
   - **Learning Outcomes**:
     - Demonstrated IaaS concepts through hands-on VM management.
     - Understood infrastructure provisioning and automation.
     - Experienced web server deployment in virtualized environment.
     - Learned about network configuration and port forwarding in cloud contexts.

## Week 05: Virtualization
### Objectives
- Understand and explain the concepts and types of virtualization
- Analyze CPU, memory, network, and storage virtualization technologies
- Compare characteristics and differences of major hypervisors
- Evaluate advantages, disadvantages, and performance considerations of virtualization technologies
- Build and manage virtualized environments

### Content
1. **Theory: Virtualization Technologies**:
   - **Virtualization Fundamentals**: Hardware abstraction, resource pooling, isolation principles.
   - **Hypervisor Types**: 
     - Type 1 (Bare-metal): VMware ESXi, Microsoft Hyper-V, Citrix XenServer.
     - Type 2 (Hosted): VMware Workstation, Oracle VirtualBox, Parallels Desktop.
   - **Virtualization Techniques**:
     - Hardware-assisted virtualization (Intel VT-x, AMD-V).
   - **Resource Management**: CPU virtualization, memory virtualization, network virtualization, storage virtualization.
   - **Virtual Machine Lifecycle**: Creation, deployment, migration, snapshotting, backup and recovery.
   - **Performance Considerations**: Overhead analysis, optimization techniques, resource contention.

2. **Hands-on Lab: Inline Vagrant Provisioning with Arch Linux**:
   - **Simple VM Configuration**: Used streamlined Vagrant setup with inline provisioning.
   - **Configuration Features**:
     ```ruby
     Vagrant.configure("2") do |config|
       config.vm.box = "dreamscapes/archlinux"
       config.vm.network "forwarded_port", guest: 22, host: 2221
       config.vm.provision "shell", inline: <<-SHELL
         echo "Hello~ IaaS: Provisioned!" >> /home/vagrant/hello.txt
         sudo pacman -Sy --noconfirm nano
       SHELL
     end
     ```

   - **Key Learning Points**:
     - **Simplified Provisioning**: Demonstrated inline shell provisioning without separate script files.
     - **Alternative OS**: Used Arch Linux instead of Ubuntu to show cross-platform virtualization.
     - **Port Forwarding**: SSH port mapping (22→2221) for remote access configuration.
     - **Package Management**: Arch Linux's pacman package manager usage in automated setup.
     - **File System Operations**: Automated file creation and text editor installation.

   - **Deployment Process**:
     1. `vagrant up`: Launch Arch Linux VM with inline provisioning.
     2. `vagrant ssh`: Connect using custom SSH port (2221).
     3. Verify provisioning: Check `/home/vagrant/hello.txt` file creation.
     4. Test installed software: Use nano text editor.
     5. `vagrant halt` and `vagrant destroy`: Lifecycle management practice.

   - **Learning Outcomes**:
     - Experienced different Linux distributions in virtualized environments.
     - Understood inline vs. external provisioning script approaches.
     - Practiced VM lifecycle management and SSH connectivity.
     - Learned cross-platform package management in automated deployments.
     - Demonstrated virtualization portability across different guest operating systems.

## Week 06: Container Technology and Docker

### Objectives

- Understand container technology concepts and differences from virtual machines
- Explain Docker architecture and core components
- Manage Docker image and container lifecycle
- Practice Docker networking and data management with volumes

### Content

1. **Theory: Container Technology and Docker**:
   
   - **Container Technology Overview**:
     - Containers package applications and dependencies into isolated environments using OS-level virtualization.
     - Emerged to solve the "it works on my machine" problem with environment consistency.
     - **Key Motivations**: Environment consistency, dependency management, deployment efficiency, resource efficiency.
   
   - **Containers vs Virtual Machines**:
     - **VM Architecture**: Physical Server → Hypervisor → Complete Guest OS per VM → Applications
     - **Container Architecture**: Physical Server → Host OS → Container Runtime → Applications
     - **Performance Comparison**: Containers achieve 78% resource utilization vs. 42% for VMs, with 28% higher throughput and 37% lower latency.
     - **Startup Time**: Containers start in 1-2 seconds vs. several minutes for VMs.
   
   - **Docker Architecture**:
     - **Client-Server Architecture** with REST API communication:
       - **Docker Client (CLI)**: User interface for Docker interaction
       - **Docker Daemon (dockerd)**: Manages Docker objects (images, containers, networks, volumes)
       - **Container Runtime**: containerd (high-level) + runc (low-level execution)
     - **Docker Objects**:
       - **Images**: Read-only templates with layered structure for efficient storage
       - **Containers**: Runnable instances of images with isolated processes
       - **Registry**: Services for storing and distributing images (Docker Hub, AWS ECR)
   
   - **Docker Image Layer System**:
     - Each Dockerfile instruction creates a new layer
     - **Layer Characteristics**: Immutability, incremental changes, shareability
     - **Union Filesystem**: Combines multiple layers using overlay2 storage driver
     - **Optimization Techniques**: Multi-stage builds, layer caching, lightweight base images (Alpine Linux)
   
   - **Docker Networking**:
     - **Bridge Network (Default)**: Isolated private network with NAT for external communication
     - **Host Network**: Direct use of host's network stack for best performance but no isolation
     - **None Network**: Complete network isolation for security-critical workloads
     - **Overlay Network**: Multi-host communication using VXLAN tunnels
   
   - **Docker Volumes and Data Management**:
     - **Volumes (Recommended)**: Docker-managed storage in `/var/lib/docker/volumes/`
     - **Bind Mounts**: Direct host directory mounting for development
     - **tmpfs Mounts**: Memory-based storage for temporary data
     - **Use Cases**: Database persistence, log collection, configuration sharing
   
   - **Container Registry**:
     - **Public**: Docker Hub with official certified images
     - **Private**: AWS ECR, Google GCR, Azure ACR, Harbor
     - **Security**: Image scanning, RBAC, Docker Content Trust for signing

2. **Hands-on Lab: Docker Installation and Basic Operations**:
   
   - **Docker Installation on Ubuntu** (`docker-install-ubuntu.md`):
     - Removed existing Docker packages to avoid conflicts
     - Set up official Docker repository with GPG key authentication
     - Installed Docker Engine components: `docker-ce`, `docker-ce-cli`, `containerd.io`, `docker-buildx-plugin`, `docker-compose-plugin`
     - Configured Docker service and enabled auto-start on boot
     - Added user to docker group for non-root access
   
   - **Basic Docker Commands**:
     ```bash
     # Test installation
     docker run hello-world
     
     # Image management
     docker pull nginx:alpine
     docker images
     docker rmi <image-id>
     
     # Container lifecycle
     docker run -d -p 8080:80 --name web nginx:alpine
     docker ps
     docker stop web
     docker start web
     docker rm web
     
     # Logs and debugging
     docker logs web
     docker exec -it web /bin/sh
     
     # System cleanup
     docker system prune -a
     ```
   
   - **Learning Outcomes**:
     - Successfully installed and configured Docker on Ubuntu
     - Understood Docker architecture through practical commands
     - Managed image and container lifecycle operations
     - Practiced port mapping and container networking basics
     - Experienced container isolation and resource management

## Week 07: Container Orchestration with Docker Compose and Kubernetes

### Objectives

- Configure multi-container applications using Docker Compose and understand its limitations
- Explain the necessity and basic concepts of Kubernetes
- Understand Kubernetes cluster architecture
- Describe core Kubernetes objects like Pod, Service, and Deployment
- Perform basic Kubernetes operations using MiniKube and kubectl

### Content

1. **Theory: Docker Compose and Kubernetes**:
   
   - **Docker Compose Basics**:
     - Tool for defining and running multi-container applications
     - **Need**: Modern apps consist of web servers, databases, caches, message queues, and load balancers
     - **docker-compose.yml Structure**:
       - `services`: Container definitions
       - `volumes`: Data persistence
       - `networks`: Inter-service communication
       - Service options: image, build, ports, environment, depends_on, volumes
     - **Workflow**: Development (service definition → execution → testing) and Production (configuration → scaling → updates → monitoring)
   
   - **Docker Compose Limitations**:
     - **Single Host Constraints**: Limited scalability and no high availability
     - **Production Limitations**: Manual recovery, limited load balancing, basic deployment strategies
     - **Monitoring**: Limited metrics and observability, simple health checks
   
   - **Introduction to Kubernetes (K8s)**:
     - Open-source platform for automating containerized application deployment, scaling, and management
     - Originally developed by Google, now managed by CNCF
     - **Problems Solved**: Service discovery, load balancing, self-healing, declarative configuration
     - **Core Concepts**:
       - **Declarative Management**: Define desired state → Monitor current state → Automatic adjustment
       - **Controller Pattern**: Continuous monitoring → Detect differences → Adjustment actions
   
   - **Kubernetes Architecture**:
     - **Control Plane (Master Node)**:
       - **API Server**: Cluster frontend with RESTful API, authentication/authorization
       - **etcd**: Distributed key-value store for cluster state using RAFT algorithm
       - **Scheduler**: Pod placement decisions based on resources and constraints
       - **Controller Manager**: Runs controllers to maintain desired state
     - **Worker Node**:
       - **kubelet**: Node agent for Pod management and status reporting
       - **kube-proxy**: Network proxy for Service abstraction and load balancing
       - **Container Runtime**: Docker, containerd, CRI-O for container execution
   
   - **Kubernetes Core Objects**:
     - **Pod**: 
       - Smallest deployable unit containing one or more containers
       - Shared network and storage within Pod
       - Ephemeral nature with unique cluster IP
       - Patterns: Single container (most common), Multi-container (sidecar pattern)
     
     - **Service**:
       - Abstracts network access to Pods (handles dynamic IP changes)
       - **Types**:
         - ClusterIP: Internal cluster access only (default)
         - NodePort: External access through node ports
         - LoadBalancer: Cloud provider integration for external traffic
     
     - **Deployment**:
       - Provides declarative updates for Pods and ReplicaSets
       - Functions: Replica management, rolling updates, rollback, scaling
       - Manages ReplicaSets which create and manage actual Pods
   
   - **MiniKube and kubectl**:
     - **MiniKube**: Single-node cluster for local development and learning
     - **kubectl**: CLI tool with syntax: `kubectl [command] [TYPE] [NAME] [flags]`
   
   - **Kubernetes vs Docker Compose Comparison**:
     - **Docker Compose Suitable For**: Development, small apps, prototyping, CI/CD
     - **Kubernetes Suitable For**: Production, microservices, high availability, multi-cloud
     - **Technical Differences**: Complexity, scalability, availability capabilities

2. **Hands-on Lab Part 1: Docker Compose Multi-Container Application** (`docker_compose.txt`):
   
   - **Project Setup**:
     - Created directory structure with `docker-compose.yml` and custom HTML
     - Defined multi-service architecture with networking and data persistence
   
   - **Service Configuration**:
     ```yaml
     services:
       web:
         image: nginx:alpine
         ports: ["8080:80"]
         volumes: ["./html:/usr/share/nginx/html:ro"]
         depends_on: [db]
       
       db:
         image: postgres:13
         environment:
           POSTGRES_DB: myapp
           POSTGRES_USER: admin
           POSTGRES_PASSWORD: secret
         volumes: [db_data:/var/lib/postgresql/data]
       
       redis:
         image: redis:alpine
     
     volumes:
       db_data:
     
     networks:
       app-net:
         driver: bridge
     ```
   
   - **Operations Practiced**:
     ```bash
     # Start multi-container application
     docker compose up -d
     
     # Check service status
     docker compose ps
     
     # View logs
     docker compose logs web
     
     # Database connection test
     docker compose exec db psql -U admin -d myapp
     
     # Scale services
     docker compose up -d --scale web=2
     
     # Cleanup
     docker compose down -v
     ```
   
   - **Learning Outcomes**:
     - Configured complex multi-container applications declaratively
     - Understood service dependencies and networking
     - Practiced volume management for data persistence
     - Experienced service scaling and inter-service communication

3. **Hands-on Lab Part 2: Kubernetes with MiniKube** (`kubectl-minikube-install-ubuntu.md`, `my-pod.yaml`):
   
   - **Installation Process**:
     - **kubectl Installation**:
       ```bash
       curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"
       chmod +x kubectl
       sudo mv kubectl /usr/local/bin/
       kubectl version --client
       ```
     
     - **MiniKube Installation**:
       ```bash
       curl -Lo minikube https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
       chmod +x minikube
       sudo mv minikube /usr/local/bin/
       minikube version
       ```
     
     - **Cluster Startup**:
       ```bash
       minikube start --driver=docker
       minikube status
       kubectl cluster-info
       kubectl get nodes
       ```
   
   - **Basic Kubernetes Operations**:
     - Created Pod definition (`my-pod.yaml`):
       ```yaml
       apiVersion: v1
       kind: Pod
       metadata:
         name: web-pod
         labels:
           app: web
       spec:
         containers:
         - name: web-container
           image: nginx:1.21
           ports:
           - containerPort: 80
       ```
     
     - **kubectl Commands Practiced**:
       ```bash
       # Pod management
       kubectl apply -f my-pod.yaml
       kubectl get pods
       kubectl describe pod web-pod
       kubectl logs web-pod
       kubectl delete pod web-pod
       
       # Imperative Pod creation
       kubectl run test-pod --image=nginx:1.21
       
       # Cluster management
       minikube dashboard
       minikube stop
       minikube start
       ```
   
   - **Troubleshooting**:
     - Configured Docker group permissions for non-root access
     - Handled MiniKube startup failures with service restarts
     - Used `minikube delete --all --purge` for clean reinstallation
   
   - **Learning Outcomes**:
     - Successfully set up local Kubernetes development environment
     - Understood Kubernetes object model through YAML definitions
     - Practiced declarative (YAML) and imperative (CLI) Pod management
     - Experienced cluster lifecycle management with MiniKube
     - Verified Kubernetes concepts learned in theory through hands-on practice
     - Prepared foundation for production-grade orchestration concepts

---

## Technologies and Tools Used

- **Version Control**: Git, GitHub
- **Cloud Platforms**: Heroku (PaaS)
- **Virtualization**: VirtualBox, Vagrant
- **Containers**: Docker, Docker Compose
- **Orchestration**: Kubernetes, MiniKube, kubectl
- **Operating Systems**: Ubuntu 24.04, Arch Linux
- **Web Servers**: Apache, Nginx
- **Databases**: PostgreSQL
- **Caching**: Redis
- **Programming**: Python (Flask), Shell scripting

## Learning Path Summary

1. **Weeks 1-2**: Cloud fundamentals and service models
2. **Week 3**: Deployment models and version control with Git
3. **Weeks 4-5**: Infrastructure concepts through virtualization
4. **Week 6**: Container technology and Docker fundamentals
5. **Week 7**: Multi-container apps and Kubernetes orchestration
6. **Week 8**: Midterm Exam