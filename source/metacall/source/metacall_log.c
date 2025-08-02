
/* -- Headers -- */

#include <metacall/metacall_log.h>

#include <log/log.h>

/* -- Methods -- */

int metacall_log(enum metacall_log_id log_id, void *ctx)
{
	switch (log_id)
	{
		case METACALL_LOG_STDIO: {
			metacall_log_stdio stdio_ctx = (metacall_log_stdio)ctx;

			return log_configure("metacall",
				log_policy_format_text_flags(LOG_POLICY_FORMAT_TEXT_NEWLINE),
				log_policy_schedule_sync(),
				log_policy_storage_sequential(),
				log_policy_stream_stdio(stdio_ctx->stream));
		}

		case METACALL_LOG_FILE: {
			metacall_log_file file_ctx = (metacall_log_file)ctx;

			return log_configure("metacall",
				log_policy_format_text_flags(LOG_POLICY_FORMAT_TEXT_NEWLINE),
				log_policy_schedule_sync(),
				log_policy_storage_sequential(),
				log_policy_stream_file(file_ctx->file_name, file_ctx->mode));
		}

		case METACALL_LOG_SOCKET: {
			metacall_log_socket socket_ctx = (metacall_log_socket)ctx;

			return log_configure("metacall",
				log_policy_format_text_flags(LOG_POLICY_FORMAT_TEXT_EMPTY),
				log_policy_schedule_sync(),
				log_policy_storage_sequential(),
				log_policy_stream_socket(socket_ctx->ip, socket_ctx->port));
		}

		case METACALL_LOG_SYSLOG: {
			metacall_log_syslog syslog_ctx = (metacall_log_syslog)ctx;

			return log_configure("metacall",
				log_policy_format_text_flags(LOG_POLICY_FORMAT_TEXT_NEWLINE),
				log_policy_schedule_sync(),
				log_policy_storage_sequential(),
				log_policy_stream_syslog(syslog_ctx->name));
		}

		case METACALL_LOG_NGINX: {
			metacall_log_nginx nginx_ctx = (metacall_log_nginx)ctx;

			return log_configure("metacall",
				log_policy_format_text_flags(LOG_POLICY_FORMAT_TEXT_EMPTY),
				log_policy_schedule_sync(),
				log_policy_storage_sequential(),
				log_policy_stream_nginx(nginx_ctx->log, nginx_ctx->log_error, nginx_ctx->log_level));
		}

		case METACALL_LOG_CUSTOM: {
			metacall_log_custom custom_ctx = (metacall_log_custom)ctx;

			return log_configure("metacall",
				log_policy_format_custom(custom_ctx->context, (log_policy_format_custom_size_ptr)custom_ctx->format_size, (log_policy_format_custom_serialize_ptr)custom_ctx->format_serialize, (log_policy_format_custom_deserialize_ptr)custom_ctx->format_deserialize),
				log_policy_schedule_sync(),
				log_policy_storage_sequential(),
				log_policy_stream_custom(custom_ctx->context, custom_ctx->stream_write, custom_ctx->stream_flush));
		}
	}

	return 1;
}
