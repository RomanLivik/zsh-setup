# Zsh Configurator
An interactive terminal tool to transform your standard Zsh into a powerful, Fish-like environment on macOS and Linux.

## Prerequisites
Ensure you have the following installed:

* GCC/G++ (supporting C++17)
* curl (to download Oh My Zsh)
* git (to clone plugins and themes)
* Zsh (the target shell)

## Installation
``` bash
git clone https://github.com/RomanLivik/zsh-setup.git
cd zsh-setup
g++ -std=c++17 zsh-setup.cpp -o zsh-setup
./zsh-setup
```
## How to Use
* **Arrows (Up/Down):** Navigate through the menu items.
* **Space:** Toggle a feature or plugin (Select/Deselect).
* **Enter:** Confirm selection and move to the next screen.

## Plugins Included
* zsh-autosuggestions
* zsh-syntax-highlighting / fast-syntax-highlighting
* zsh-completions
* git, sudo, extract, z (OMZ built-ins)
