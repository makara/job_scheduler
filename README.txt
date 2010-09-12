$Id$

Job Scheduler
=============

Job Scheduler is an API module that offers a scheduler for dispatching tasks
in a timed matter once or periodically.


Usage
=====

Add a job.

  $job = array(
    'callback' => 'mymodule_unpublish_nodes',
    'type' => 'story',
    'id' => 12,
    'period' => 3600,
    'periodic' => TRUE,
  );
  job_scheduler()->set($job);

Work off a job

  function mymodule_unpublish_nodes($job) {
    // Do stuff.
  }

Remove a job.

  $job = array(
    'callback' => 'mymodule_unpublish_nodes',
    'type' => 'story',
    'id' => 12,
  );
  job_scheduler()->remove($job);


Drupal Queue integration
========================

If Drupal Queue is installed, tasks are dispatched to the queue rather than
executed. Hence worker callbacks need to be declared to DrupalQueue via
hook_cron_queue_info().


Example
=======

See Feeds module.


Hidden settings
===============

Hidden settings are variables that you can define by adding them to the $conf
array in your settings.php file.


Name:        job_scheduler_class
Default:     'JobScheduler'
Description: The class to use for managing schedule.

Name:        job_schedule_num
Default:     5
Description: The number of jobs to execute on cron time.
             Only has an effect if Drupal Queue is *not* enabled.
             http://drupal.org/project/drupal_queue

Name:        job_schedule_queue_num
Default:     200
Description: The number of jobs to queue for execution on cron time. Only has
             an effect if Drupal Queue is enabled.
             http://drupal.org/project/drupal_queue
