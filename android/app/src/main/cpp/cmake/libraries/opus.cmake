# Internal Opus codec configuration
include_guard(GLOBAL)

option(USE_CODEC_OPUS "Ogg Opus support" ON)
option(USE_INTERNAL_OPUS "Use internal copy of Opus" ON)

if(NOT USE_CODEC_OPUS)
    return()
endif()

list(APPEND CLIENT_DEFINITIONS USE_CODEC_OPUS)

if(NOT USE_INTERNAL_OPUS)
    return()
endif()

message(STATUS "Using internal opus-1.2.1")

set(OPUS_DIR ${SOURCE_DIR}/opus-1.2.1)
set(OPUSFILE_DIR ${SOURCE_DIR}/opusfile-0.9)
set(OGG_DIR ${SOURCE_DIR}/libogg-1.3.3)

set(OPUS_SOURCES
    # Opus core
    ${OPUS_DIR}/src/analysis.c
    ${OPUS_DIR}/src/mlp.c
    ${OPUS_DIR}/src/mlp_data.c
    ${OPUS_DIR}/src/opus.c
    ${OPUS_DIR}/src/opus_decoder.c
    ${OPUS_DIR}/src/opus_encoder.c
    ${OPUS_DIR}/src/opus_multistream.c
    ${OPUS_DIR}/src/opus_multistream_encoder.c
    ${OPUS_DIR}/src/opus_multistream_decoder.c
    ${OPUS_DIR}/src/repacketizer.c

    # CELT
    ${OPUS_DIR}/celt/bands.c
    ${OPUS_DIR}/celt/celt.c
    ${OPUS_DIR}/celt/cwrs.c
    ${OPUS_DIR}/celt/entcode.c
    ${OPUS_DIR}/celt/entdec.c
    ${OPUS_DIR}/celt/entenc.c
    ${OPUS_DIR}/celt/kiss_fft.c
    ${OPUS_DIR}/celt/laplace.c
    ${OPUS_DIR}/celt/mathops.c
    ${OPUS_DIR}/celt/mdct.c
    ${OPUS_DIR}/celt/modes.c
    ${OPUS_DIR}/celt/pitch.c
    ${OPUS_DIR}/celt/celt_encoder.c
    ${OPUS_DIR}/celt/celt_decoder.c
    ${OPUS_DIR}/celt/celt_lpc.c
    ${OPUS_DIR}/celt/quant_bands.c
    ${OPUS_DIR}/celt/rate.c
    ${OPUS_DIR}/celt/vq.c

    # SILK
    ${OPUS_DIR}/silk/CNG.c
    ${OPUS_DIR}/silk/code_signs.c
    ${OPUS_DIR}/silk/init_decoder.c
    ${OPUS_DIR}/silk/decode_core.c
    ${OPUS_DIR}/silk/decode_frame.c
    ${OPUS_DIR}/silk/decode_parameters.c
    ${OPUS_DIR}/silk/decode_indices.c
    ${OPUS_DIR}/silk/decode_pulses.c
    ${OPUS_DIR}/silk/decoder_set_fs.c
    ${OPUS_DIR}/silk/dec_API.c
    ${OPUS_DIR}/silk/enc_API.c
    ${OPUS_DIR}/silk/encode_indices.c
    ${OPUS_DIR}/silk/encode_pulses.c
    ${OPUS_DIR}/silk/gain_quant.c
    ${OPUS_DIR}/silk/interpolate.c
    ${OPUS_DIR}/silk/LP_variable_cutoff.c
    ${OPUS_DIR}/silk/NLSF_decode.c
    ${OPUS_DIR}/silk/NSQ.c
    ${OPUS_DIR}/silk/NSQ_del_dec.c
    ${OPUS_DIR}/silk/PLC.c
    ${OPUS_DIR}/silk/shell_coder.c
    ${OPUS_DIR}/silk/tables_gain.c
    ${OPUS_DIR}/silk/tables_LTP.c
    ${OPUS_DIR}/silk/tables_NLSF_CB_NB_MB.c
    ${OPUS_DIR}/silk/tables_NLSF_CB_WB.c
    ${OPUS_DIR}/silk/tables_other.c
    ${OPUS_DIR}/silk/tables_pitch_lag.c
    ${OPUS_DIR}/silk/tables_pulses_per_block.c
    ${OPUS_DIR}/silk/VAD.c
    ${OPUS_DIR}/silk/control_audio_bandwidth.c
    ${OPUS_DIR}/silk/quant_LTP_gains.c
    ${OPUS_DIR}/silk/VQ_WMat_EC.c
    ${OPUS_DIR}/silk/HP_variable_cutoff.c
    ${OPUS_DIR}/silk/NLSF_encode.c
    ${OPUS_DIR}/silk/NLSF_VQ.c
    ${OPUS_DIR}/silk/NLSF_unpack.c
    ${OPUS_DIR}/silk/NLSF_del_dec_quant.c
    ${OPUS_DIR}/silk/process_NLSFs.c
    ${OPUS_DIR}/silk/stereo_LR_to_MS.c
    ${OPUS_DIR}/silk/stereo_MS_to_LR.c
    ${OPUS_DIR}/silk/check_control_input.c
    ${OPUS_DIR}/silk/control_SNR.c
    ${OPUS_DIR}/silk/init_encoder.c
    ${OPUS_DIR}/silk/control_codec.c
    ${OPUS_DIR}/silk/A2NLSF.c
    ${OPUS_DIR}/silk/ana_filt_bank_1.c
    ${OPUS_DIR}/silk/biquad_alt.c
    ${OPUS_DIR}/silk/bwexpander_32.c
    ${OPUS_DIR}/silk/bwexpander.c
    ${OPUS_DIR}/silk/debug.c
    ${OPUS_DIR}/silk/decode_pitch.c
    ${OPUS_DIR}/silk/inner_prod_aligned.c
    ${OPUS_DIR}/silk/lin2log.c
    ${OPUS_DIR}/silk/log2lin.c
    ${OPUS_DIR}/silk/LPC_analysis_filter.c
    ${OPUS_DIR}/silk/LPC_fit.c
    ${OPUS_DIR}/silk/LPC_inv_pred_gain.c
    ${OPUS_DIR}/silk/table_LSF_cos.c
    ${OPUS_DIR}/silk/NLSF2A.c
    ${OPUS_DIR}/silk/NLSF_stabilize.c
    ${OPUS_DIR}/silk/NLSF_VQ_weights_laroia.c
    ${OPUS_DIR}/silk/pitch_est_tables.c
    ${OPUS_DIR}/silk/resampler.c
    ${OPUS_DIR}/silk/resampler_down2_3.c
    ${OPUS_DIR}/silk/resampler_down2.c
    ${OPUS_DIR}/silk/resampler_private_AR2.c
    ${OPUS_DIR}/silk/resampler_private_down_FIR.c
    ${OPUS_DIR}/silk/resampler_private_IIR_FIR.c
    ${OPUS_DIR}/silk/resampler_private_up2_HQ.c
    ${OPUS_DIR}/silk/resampler_rom.c
    ${OPUS_DIR}/silk/sigm_Q15.c
    ${OPUS_DIR}/silk/sort.c
    ${OPUS_DIR}/silk/sum_sqr_shift.c
    ${OPUS_DIR}/silk/stereo_decode_pred.c
    ${OPUS_DIR}/silk/stereo_encode_pred.c
    ${OPUS_DIR}/silk/stereo_find_predictor.c
    ${OPUS_DIR}/silk/stereo_quant_pred.c

    # SILK float
    ${OPUS_DIR}/silk/float/apply_sine_window_FLP.c
    ${OPUS_DIR}/silk/float/corrMatrix_FLP.c
    ${OPUS_DIR}/silk/float/encode_frame_FLP.c
    ${OPUS_DIR}/silk/float/find_LPC_FLP.c
    ${OPUS_DIR}/silk/float/find_LTP_FLP.c
    ${OPUS_DIR}/silk/float/find_pitch_lags_FLP.c
    ${OPUS_DIR}/silk/float/find_pred_coefs_FLP.c
    ${OPUS_DIR}/silk/float/LPC_analysis_filter_FLP.c
    ${OPUS_DIR}/silk/float/LTP_analysis_filter_FLP.c
    ${OPUS_DIR}/silk/float/LTP_scale_ctrl_FLP.c
    ${OPUS_DIR}/silk/float/noise_shape_analysis_FLP.c
    ${OPUS_DIR}/silk/float/process_gains_FLP.c
    ${OPUS_DIR}/silk/float/regularize_correlations_FLP.c
    ${OPUS_DIR}/silk/float/residual_energy_FLP.c
    ${OPUS_DIR}/silk/float/warped_autocorrelation_FLP.c
    ${OPUS_DIR}/silk/float/wrappers_FLP.c
    ${OPUS_DIR}/silk/float/autocorrelation_FLP.c
    ${OPUS_DIR}/silk/float/burg_modified_FLP.c
    ${OPUS_DIR}/silk/float/bwexpander_FLP.c
    ${OPUS_DIR}/silk/float/energy_FLP.c
    ${OPUS_DIR}/silk/float/inner_product_FLP.c
    ${OPUS_DIR}/silk/float/k2a_FLP.c
    ${OPUS_DIR}/silk/float/LPC_inv_pred_gain_FLP.c
    ${OPUS_DIR}/silk/float/pitch_analysis_core_FLP.c
    ${OPUS_DIR}/silk/float/scale_copy_vector_FLP.c
    ${OPUS_DIR}/silk/float/scale_vector_FLP.c
    ${OPUS_DIR}/silk/float/schur_FLP.c
    ${OPUS_DIR}/silk/float/sort_FLP.c
)

# Opusfile sources (separate since they need different includes)
set(OPUSFILE_SOURCES
    ${OPUSFILE_DIR}/src/http.c
    ${OPUSFILE_DIR}/src/info.c
    ${OPUSFILE_DIR}/src/internal.c
    ${OPUSFILE_DIR}/src/opusfile.c
    ${OPUSFILE_DIR}/src/stream.c
    ${OPUSFILE_DIR}/src/wincerts.c
)

# Create object library for opus with opus-specific includes to avoid mdct.h conflict with vorbis
# Both vorbis and opus define mdct_lookup with different structures
add_library(opus_objects OBJECT ${OPUS_SOURCES})
target_include_directories(opus_objects PRIVATE
    ${OPUS_DIR}/include
    ${OPUS_DIR}/celt
    ${OPUS_DIR}/silk
    ${OPUS_DIR}/silk/float
)
target_compile_definitions(opus_objects PRIVATE
    OPUS_BUILD
    HAVE_LRINTF
    FLOATING_POINT
    FLOAT_APPROX
    USE_ALLOCA
)
target_compile_options(opus_objects PRIVATE ${COMMON_COMPILE_OPTIONS})

# Create object library for opusfile (needs both opus and ogg includes)
add_library(opusfile_objects OBJECT ${OPUSFILE_SOURCES})
target_include_directories(opusfile_objects PRIVATE
    ${OPUSFILE_DIR}/include
    ${OPUS_DIR}/include
    ${OGG_DIR}/include
)
target_compile_options(opusfile_objects PRIVATE ${COMMON_COMPILE_OPTIONS})

# Add the opus/opusfile include directories for the main client (for opus.h, opusfile.h etc)
# Do NOT add celt/ or silk/ to global includes as celt/ contains mdct.h which conflicts with vorbis
list(APPEND CLIENT_INCLUDE_DIRS
    ${OPUS_DIR}/include
    ${OPUSFILE_DIR}/include
)

# Export the object libraries to link with client
list(APPEND CLIENT_OBJECT_LIBRARIES opus_objects opusfile_objects)
