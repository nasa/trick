class Trick < Formula
  desc "Simulation development framework"
  homepage "https://github.com/nasa/trick"
  url "https://github.com/nasa/trick/archive/refs/tags/25.0.2.tar.gz"
  sha256 "0e51ee68d7239423a18b144dea9e0560125c523a0e2d8a914933ace1bba39dd3"
  license "NASA-1.3"
  head "https://github.com/nasa/trick.git", branch: "master"
  revision 1

  depends_on "pkgconf" => :build
  depends_on "bison"
  depends_on "gsl"
  depends_on "hdf5"
  depends_on "libaec"
  depends_on "libx11"
  depends_on "libxt"
  depends_on "llvm"
  depends_on :macos
  depends_on "maven"
  depends_on "openjdk"
  depends_on "openmotif"
  depends_on "perl"
  depends_on "python"
  depends_on "swig"
  depends_on "udunits"
  depends_on "zlib"

  uses_from_macos "flex"
  uses_from_macos "libxml2"

  def install
    system "./configure",
           "--with-gsl=#{Formula["gsl"].opt_prefix}",
           "--with-hdf5=#{Formula["hdf5"].opt_prefix}",
           "--with-udunits=#{Formula["udunits"].opt_prefix}"
    system "make", "-j#{ENV.make_jobs}"

    bin.install Dir["bin/*"] if File.directory?("bin")
    lib.install Dir["lib/*"] if File.directory?("lib")
    include.install Dir["include/*"] if File.directory?("include")
    share.install Dir["share/*"] if File.directory?("share")
    libexec.install Dir["libexec/*"] if File.directory?("libexec")

    # Install trick_source/er7_utils
    (prefix/"trick_source").install "trick_source/er7_utils"

    # Clean up build artifacts
    rm_r(Dir[prefix/"trick_source/**/object_*"])
    rm_r(Dir[prefix/"trick_source/**/io_src"])

    inreplace pkgshare/"makefiles/config_user.mk" do |s|
      # Fix hardcoded shim compiler paths in installed config file
      s.gsub! "super/clang++", "clang++"
      s.gsub! "super/clang", "clang"
      s.gsub! "super/ld", "ld"
      s.gsub! %r{#{HOMEBREW_SHIMS_PATH}/[^/]+/}o, ""
      # Replace javac with homebrew openjdk javac path
      s.gsub! "javac", "#{Formula["openjdk"].opt_bin}/javac"
    end

    # Fix HDF5 library paths to include libaec for libsz
    inreplace pkgshare/"makefiles/Makefile.common",
      "HDF5_LIB := -L$(HDF5)/lib -lhdf5_hl -lhdf5 -lsz",
      "HDF5_LIB := -L$(HDF5)/lib -L#{Formula["libaec"].opt_lib} -lhdf5_hl -lhdf5 -lsz"
  end

  test do
    output = shell_output("#{bin}/trick-CP --help", 1)
    assert_match "Trick Configuration Processor", output
  end
end
