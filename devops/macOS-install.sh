#!/bin/zsh

# Trick Installation Script for macOS
# This script installs Homebrew (if needed) and Trick with all dependencies

set -u  # Exit on unset variable

TRICK_TAP="brendan-nasa/trick"
TRICK_PACKAGE="trick"
LOG_FILE="$HOME/trick_install_log.txt"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}==>${NC} $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}!${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" >> "$LOG_FILE"
}

check_macos() {
    if [[ "$OSTYPE" != "darwin"* ]]; then
        print_error "This script is designed for macOS only."
        print_error "Current OS: $OSTYPE"
        exit 1
    fi
}

confirm_proceed() {
    local response
    read -r "response?Do you want to proceed? (y/n): "
    response="${response:l}"

    if [[ "$response" == "y" || "$response" == "yes" ]]; then
        return 0
    else
        echo "Installation cancelled."
        exit 0
    fi
}

# add Homebrew to PATH for current session
setup_homebrew_path() {
    if [[ -f "/opt/homebrew/bin/brew" ]]; then
        # Apple Silicon Mac
        eval "$(/opt/homebrew/bin/brew shellenv)"
        print_status "Added Homebrew to PATH (Apple Silicon)"
    elif [[ -f "/usr/local/bin/brew" ]]; then
        # Intel Mac
        eval "$(/usr/local/bin/brew shellenv)"
        print_status "Added Homebrew to PATH (Intel)"
    fi
}

install_homebrew() {
    print_status "Homebrew not found. Installing Homebrew..."
    print_warning "This may take 5-10 minutes. Please be patient..."
    echo ""

    log_message "Starting Homebrew installation"

    if /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"; then
        log_message "Homebrew installation completed"

        setup_homebrew_path

        if command -v brew &> /dev/null; then
            print_success "Homebrew installed successfully!"
            echo ""
            return 0
        else
            print_error "Homebrew was installed but cannot be found in PATH"
            print_warning "Try closing and reopening your terminal, then run this script again"
            log_message "Homebrew installation succeeded but not found in PATH"
            exit 1
        fi
    else
        print_error "Homebrew installation failed"
        print_warning "Please check the error messages above"
        # print_warning "You may need to install Xcode Command Line Tools first:"
        # echo "          xcode-select --install"
        # log_message "Homebrew installation failed"
        exit 1
    fi
}

add_trick_tap() {
    print_status "Adding Trick software repository to Homebrew..."
    log_message "Adding tap: $TRICK_TAP"

    brew tap "$TRICK_TAP" 2>&1 | tee -a "$LOG_FILE"
    local tap_status=${pipestatus[1]}

    if [ $tap_status -eq 0 ]; then
        if brew tap | grep -q "$TRICK_TAP"; then
            print_success "Trick repository added successfully"
            echo ""
            return 0
        else
            print_error "Repository tap completed but verification failed"
            log_message "Tap verification failed"
            exit 1
        fi
    else
        print_error "Failed to add Trick repository (exit code: $tap_status)"
        print_warning "Please check your internet connection and try again"
        log_message "Failed to tap $TRICK_TAP with exit code: $tap_status"
        exit 1
    fi
}

install_trick() {
    print_status "Installing Trick and its dependencies..."
    echo ""

    log_message "Starting Trick installation"

    brew install "$TRICK_PACKAGE" 2>&1 | tee -a "$LOG_FILE"
    local brew_status=${pipestatus[1]}

    if [ $brew_status -eq 0 ]; then
        print_success "Trick installed successfully!"
        echo ""
        log_message "Trick installation completed"
        return 0
    else
        print_error "Trick installation failed (exit code: $brew_status)"
        print_warning "Check the log file for details: $LOG_FILE"
        log_message "Trick installation failed with exit code: $brew_status"
        exit 1
    fi
}

verify_trick() {
    print_status "Verifying Trick installation..."

    if brew list "$TRICK_PACKAGE" &> /dev/null; then
        print_success "Trick is installed correctly"

        if command -v trick-CP &> /dev/null; then
            echo ""
            print_status "You can now use Trick simulation environment"
        fi
        return 0
    else
        print_warning "Trick may not have installed correctly"
        return 1
    fi
}

main() {
    log_message "=== Trick Installation Script Started ==="

    check_macos

    cat <<EOS

╔════════════════════════════════════════════════════════════╗
║           Trick Simulation Environment Installer           ║
╚════════════════════════════════════════════════════════════╝

This script will install the Trick simulation environment on
your Mac. Here's what will happen:

  1. Check if Homebrew is installed (or install it)
  2. Add the Trick software repository
  3. Install Trick and all required dependencies

Note: You will need administrator privileges to install Homebrew.

A log file will be created at:
$LOG_FILE

EOS

    confirm_proceed
    echo ""

    if command -v brew &> /dev/null; then
        print_success "Homebrew is already installed"
        log_message "Homebrew already installed: $(brew --version | head -n1)"
        echo ""
    else
        install_homebrew
    fi

    if brew list "$TRICK_PACKAGE" &> /dev/null 2>&1; then
        print_warning "Trick is already installed on this system"
        echo ""
        echo "Would you like to:"
        echo "  1) Reinstall Trick"
        echo "  2) Upgrade Trick to latest version"
        echo "  3) Exit without changes"
        echo ""
        read -r "choice?Enter your choice (1-3): "

        case "$choice" in
            1)
                print_status "Reinstalling Trick..."
                brew reinstall "$TRICK_PACKAGE" 2>&1 | tee -a "$LOG_FILE"
                local reinstall_status=${pipestatus[1]}
                [ $reinstall_status -ne 0 ] && print_error "Reinstall failed" && exit 1
                ;;
            2)
                print_status "Upgrading Trick..."
                brew upgrade "$TRICK_PACKAGE" 2>&1 | tee -a "$LOG_FILE"
                local upgrade_status=${pipestatus[1]}
                [ $upgrade_status -ne 0 ] && print_error "Upgrade failed" && exit 1
                ;;
            3)
                echo "No changes made. Exiting..."
                exit 0
                ;;
            *)
                print_warning "Invalid choice. Exiting..."
                exit 1
                ;;
        esac
    else
        # Fresh install
        add_trick_tap
        install_trick
        verify_trick
    fi

    echo ""
    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║                   Installation Complete!                   ║"
    echo "╚════════════════════════════════════════════════════════════╝"
    echo ""
    print_success "Trick has been installed successfully"
    echo ""
    echo "Next steps:"
    echo "  • Close and reopen your terminal"
    echo "  • Run 'brew info trick' to see installation details"
    echo "  • Check the documentation to start using Trick"
    echo ""
    echo "Installation log saved to: $LOG_FILE"
    echo ""

    log_message "=== Trick Installation Script Completed Successfully ==="
}

main
