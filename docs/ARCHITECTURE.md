# Unix Epoch Clock — architecture and Common dependency

The Clock-Unix repository owns its provider identity, formatting, tick policy, tests and releases.
Common is the only owner of reusable arithmetic, temporal coordinates, provider ABI and validation.

## Real Common dependency

The common/ Git submodule is pinned to Common 1.19.80 at dc4aac326964ca4400b6e21851d16eea674040c8.
The provider includes Common public headers and compiles against its actual static archive,
never private Common source files or copied ABI headers. The Common archive is compiled
under the same portable or aggressive optimisation profile as the provider.

Unix second conversion uses infiltratr_i64_floor_divmod; phase arithmetic uses
infiltratr_temporal_local_microseconds_of_day. The two resource entry points remain
the only dynamically exported provider symbols.

The standalone native installer and source ZIP snapshot the exact reviewed Common
submodule source, so installation builds offline without depending on a separately
packaged Common runtime library. The submodule ensures shared source changes are
reviewable and pinned rather than independently copied across provider repositories.

The original source-copy provenance in SOURCE-MANIFEST.json is retained as history.
Provider regression and independent accuracy fixtures remain mandatory.
