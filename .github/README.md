# GitHub Actions Workflows

This directory contains GitHub Actions workflows for continuous integration, releases, and code quality checks for the InterObfu project.

## Workflows Overview

### 🔄 CI Workflow (`ci.yml`)

**Triggers:** Push to main/master/develop branches, Pull Requests

**Features:**
- **Multi-platform builds**: Ubuntu, Windows, macOS
- **Multiple build types**: Debug and Release
- **Dependency caching**: Speeds up builds by caching CMake dependencies
- **Artifact uploads**: Stores build artifacts for download
- **Cross-platform compatibility**: Tests the cmkr build system on all platforms

**Matrix Strategy:**
```yaml
os: [ubuntu-latest, windows-latest, macos-latest]
build_type: [Debug, Release]
```

**Outputs:**
- Build artifacts for each platform/configuration
- Build status verification across platforms

### 🚀 Release Workflow (`release.yml`)

**Triggers:** 
- Git tags matching `v*` pattern (e.g., `v1.0.0`)
- Manual dispatch with version input

**Features:**
- **Automated releases**: Creates GitHub releases from tags
- **Multi-platform packages**: Linux, Windows, macOS distribution packages
- **Complete release bundles**: Includes headers, libraries, and documentation
- **Release notes**: Auto-generated release descriptions

**Release Packages Include:**
- `lib/`: Static library files
- `include/`: Header files for development  
- `docs/`: Documentation and examples
- `cmake.toml`: Build configuration
- `README.txt`: Usage instructions

### ✅ Pull Request Checks (`pr-checks.yml`)

**Triggers:** Pull Requests to main/master/develop branches

**Quality Checks:**
- **Build verification**: Ensures PR doesn't break builds
- **Code quality**: Checks for TODOs, debug prints, header guards
- **Documentation**: Verifies documentation completeness
- **Dependencies**: Analyzes dependency changes
- **Platform compatibility**: Checks for platform-specific code
- **Change summary**: Provides overview of PR modifications

**Check Categories:**
1. **Build Check**: Fast build verification
2. **Code Quality**: Static analysis and best practices
3. **Documentation**: Documentation completeness
4. **Dependencies**: Dependency analysis
5. **Changes Summary**: Change impact assessment

## Usage

### Running CI Locally

To test the build locally before pushing:

```bash
# Bootstrap cmkr
cmake -P cmkr.cmake

# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build build --target interobfu
```

### Creating a Release

1. **Tag a release:**
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```

2. **Manual release:**
   - Go to Actions tab in GitHub
   - Select "Release" workflow
   - Click "Run workflow"
   - Enter version number

### Monitoring Builds

- **CI Status**: Check the Actions tab for build status
- **Build Artifacts**: Download from the Actions run page
- **Release Assets**: Available on the Releases page

## Configuration

### Environment Requirements

**Ubuntu:**
- build-essential
- git
- CMake 3.15+

**macOS:**
- Xcode Command Line Tools
- git
- CMake 3.15+

**Windows:**
- Visual Studio Build Tools
- git
- CMake 3.15+

### Secrets Required

For releases to work properly, ensure these secrets are configured:

- `GITHUB_TOKEN`: Automatically provided by GitHub Actions

### Customization

To modify the workflows:

1. **Add new platforms**: Update the matrix in `ci.yml`
2. **Change build types**: Modify `build_type` array
3. **Add new checks**: Extend `pr-checks.yml` with additional quality gates
4. **Modify release packages**: Update the packaging steps in `release.yml`

## Workflow Files

| File | Purpose | Triggers |
|------|---------|----------|
| `ci.yml` | Continuous Integration | Push, PR |
| `release.yml` | Automated Releases | Tags, Manual |
| `pr-checks.yml` | Pull Request Validation | PR only |

## Build Artifacts

### CI Artifacts
- `interobfu-linux-Debug`
- `interobfu-linux-Release`
- `interobfu-windows-Debug` 
- `interobfu-windows-Release`
- `interobfu-macos-Debug`
- `interobfu-macos-Release`

### Release Assets
- `interobfu-linux.tar.gz`
- `interobfu-windows.zip`
- `interobfu-macos.tar.gz`

## Troubleshooting

### Common Issues

1. **Build failures**: Check dependency installation steps
2. **Missing artifacts**: Verify build target names match expectations
3. **Platform-specific issues**: Review platform conditional steps

### Debug Information

Each workflow provides detailed logging:
- Build configuration details
- Dependency installation logs
- Compilation output
- Artifact verification

### Getting Help

- Check the Actions tab for detailed logs
- Review workflow YAML files for configuration
- Ensure cmkr.cmake and cmake.toml are properly configured

## Future Enhancements

Potential improvements:
- Add test execution when tests are working
- Integrate code coverage reporting
- Add performance benchmarks
- Include security scanning
- Add documentation generation (Doxygen)