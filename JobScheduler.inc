<?php
// $Id$

/**
 * @file
 * JobScheduler class.
 */

/**
 * Handle adding and removing jobs from schedule.
 */
class JobScheduler {
  /**
   * Create a single instance of JobScheduler.
   */
  public static function instance() {
    static $instance;
    if (!isset($instance)) {
      $class = variable_get('job_scheduler_class', 'JobScheduler');
      $instance = new $class();
    }
    return $instance;
  }

  /**
   * Protect constructor.
   */
  protected function __construct() {}

  /**
   * Add a job to the schedule, replace any existing job.
   *
   * A job is uniquely identified by $job = array(callbac, type, id).
   *
   * @param $job
   *   An array that must contain the following keys:
   *   'callback' - The callback to evoke.
   *   'type'     - A string identifier of the type of job.
   *   'id'       - A numeric identifier of the job.
   *   'period'   - The time when the task should be executed.
   *   'periodic' - True if the task should be repeated periodically.
   *
   * @code
   * function callback($job) {
   *   // Work off job.
   *   // Set next time to be called. If this portion of the code is not
   *   // reached for some reason, the scheduler will keep periodically invoking
   *   // the callback() with the period value initially specified.
   *   $scheduler->set($job);
   * }
   */
  public function set($job) {
    $job['last'] = REQUEST_TIME;
    $job['next'] = REQUEST_TIME + $job['period'];
    $job['scheduled'] = 0;
    $this->remove($job);
    drupal_write_record('job_schedule', $job);
  }

  /**
   * Remove a job from the schedule, replace any existing job.
   *
   * A job is uniquely identified by $job = array(callbac, type, id).
   */
  public function remove($job) {
    db_query("DELETE FROM {job_schedule} WHERE callback = '%s' AND type = '%s' AND id = %d", $job['callback'], $job['type'], isset($job['id']) ? $job['id'] : 0);
  }

  /**
   * Remove all jobs for a given callback and type.
   */
  public function removeAll($callback, $type) {
    db_query("DELETE FROM {job_schedule} WHERE callback = '%s' AND type = '%s'", $callback, $type);
    return db_affected_rows();
  }

  /**
   * Periodic cron task.
   *
   * Dispatches jobs until a limit of 200 jobs or 30 seconds processing time is
   * reached.
   */
  public function cron() {return;
    // Check and set scheduler semaphore, take time.
    if (variable_get('job_scheduler_cron', FALSE)) {
      watchdog('JobScheduler', 'Last cron process did not finish.', array(), WATCHDOG_ERROR);
    }
    variable_set('job_scheduler_cron', TRUE);
    $start = time();

    // Reschedule stuck periodic jobs after one hour.
    db_update('job_schedule')
    ->fields(array(
      'scheduled' => 0,
    ))
    ->condition('scheduled', REQUEST_TIME - 3600, '<')
    ->condition('periodic', 1)
    ->execute();

    // Query and dispatch scheduled jobs.
    $start = time();
    $jobs = db_select('job_schedule')
      ->fields('job_schedule', array('*'))
      ->condition('scheduled', 0)
      ->condition('next', REQUEST_TIME, '<')
      ->orderBy('next', 'ASC')
      ->range(0, 200)
      ->execute();
    foreach ($jobs as $job) {
      // Flag periodic jobs as scheduled, remove one-off jobs.
      if ($job['periodic']) {
        $job['scheduled'] = $job['last'] = REQUEST_TIME;
        $job['next'] = $job['period'] + REQUEST_TIME;
        drupal_write_record('job_schedule', $job, array('callback', 'type', 'id'));
      }
      else {
        $this->remove($job);
      }
      // Queue job if there is a queue declared for it, otherwise execute it.
      if (function_exists($job['callback'])) {
        if (!$this->queue($job)) {
          $job['callback']($job);
        }
      }
      if (time() > ($start + 30)) {
        break;
      }
    }

    // Unflag and post a message that we're done.
    variable_set('job_scheduler_cron', FALSE);
    watchdog('JobScheduler', 'Finished processing schedule after !time.', array('!time' => format_interval(time() - $start)));
  }

  /**
   * Add a job to a queue.
   *
   * @todo: Implement.
   */
  protected function queue($job) {
    return FALSE;
  }
}
