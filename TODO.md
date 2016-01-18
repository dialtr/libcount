## TODO List

# Required

* The 'empirical data' unit test currently fails, due to what appears to
  be a legitimate error in the empirical data that was published as part
  of the Appendix to the HyperLogLog++ paper by Nunkesser, et al. The
  bug would result in ambiguity in choosing interpolation indices when
  calculating the bias correction.

* Provide better unit test coverage.

* Create manpages, add them to install.

# Optional
* Document downloading and building OpenSSL for MacOSX
